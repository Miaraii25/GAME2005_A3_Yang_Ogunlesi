#include <ctime>

#include "PlayScene2.h"
#include "Game.h"
#include "EventManager.h"
#include "Util.h"

#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"

PlayScene2::PlayScene2()
{
	m_frameCount = 0;
	m_wallFriction = 0.9f;
	m_drawDebug = false;
	m_drawWalls = true;
	m_ballType = BallType::CIRCLE;
	m_activeBallType[CIRCLE] = true;
	PlayScene2::start();
}

PlayScene2::PlayScene2(const SceneState lastScene)
	: PlayScene2()
{
	m_lastScene = lastScene;
}

PlayScene2::~PlayScene2()
= default;

void PlayScene2::draw()
{
	TextureManager::Instance()->draw("space", 400, 300, 0, 255, true);

	//draw debug stuff first
	if (m_drawWalls)
	{
		for (auto border : m_pBorders)
		{
			auto diff = glm::vec2(border->getWidth(), border->getHeight()) - border->getPosition();
			Util::DrawRect(border->getPosition(), diff.x, diff.y);
		}
	}

	//draw the collision debug info
	if (m_drawDebug)
	{
		float circleRadius = std::max(m_pBall->getWidth() * 0.5f, m_pBall->getHeight() * 0.5f);
		glm::vec2 circleCenter = m_pBall->getPosition();
		glm::vec2 halfExt(m_pPlayerPaddle->getWidth() / 2.0f, m_pPlayerPaddle->getHeight() / 2.0f);
		glm::vec2 aabbCenter = m_pPlayerPaddle->getPosition();

		glm::vec2 diff = circleCenter - aabbCenter;
		glm::vec2 clamped = glm::clamp(diff, -halfExt, halfExt);
		glm::vec2 closest = aabbCenter + clamped;
		diff = closest - circleCenter;

		auto surfaceNormal = glm::vec2(clamped.y, -clamped.x);

		//the collision volumes
		Util::DrawCircle(circleCenter, circleRadius + 1.0f);
		Util::DrawRect(aabbCenter - halfExt, halfExt.x * 2 + 1.0f, halfExt.y * 2 + 1.0f);

		//the displacement between the two objects
		Util::DrawLine(circleCenter, aabbCenter);

		//the point on the paddle that is closest to the ball
		Util::DrawLine(aabbCenter, closest, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		//the normal of the closest point
		Util::DrawLine(closest, closest + surfaceNormal, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

		//the velocity vector (clamped so it doesn't go crazy)
		Util::DrawLine(aabbCenter, aabbCenter + glm::clamp(m_pPlayerPaddle->getVelocity(), glm::vec2(-100.0f, -100.0f), glm::vec2(100.0f, 100.0f)), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	}

	//draw main game items
	drawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);

	//draw gui
	if (EventManager::Instance().isIMGUIActive())
	{
		GUI_Function();
	}
}

void PlayScene2::update()
{
	//do any physics checks

	//keep the player paddle in the frame
	glm::vec2 disp = m_playZone[1] - m_playZone[0];
	if (CollisionManager::pointRectCheck(EventManager::Instance().getMousePosition(), m_playZone[0], disp.x, disp.y))
	{
		//only add position every 2 frames to get smoother speed from position delta
		if (m_frameCount + 1 % 2)
		{
			m_pPlayerPaddle->addToFlightPath(EventManager::Instance().getMousePosition());
		}
	}

	if (!m_pBall->isPaused() || m_canHitWhilePaused)
	{
		//bounce the ball off the walls with a simple vector reflection

		for (auto border : m_pBorders)
		{
			CollisionResult res;

			switch (m_ballType)
			{
			case BallType::CIRCLE:
				res = CollisionManager::circleAABBCheck(m_pBall, border, [this, border](CollisionResult result) -> void
				{
					if (!m_pBall->AtRest())
					{
						glm::vec2 n = glm::normalize(result.surfaceNormal);
						glm::vec2 d = m_pBall->getVelocity();
						glm::vec2 r = d - 2.0f * (glm::dot(d, n) * n);

						m_pBall->getRigidBody()->velocity = r;
						m_pBall->getRigidBody()->velocity *= (1.0f / m_pBall->getMass());
					}
				});

				break;

			case BallType::SQUARE:
				res = CollisionManager::AABBCheck(m_pBall, border, [this, border](CollisionResult result) -> void
				{
					if (!m_pBall->AtRest())
					{
						glm::vec2 n = glm::normalize(result.surfaceNormal);
						glm::vec2 d = m_pBall->getVelocity();
						glm::vec2 r = d - 2.0f * (glm::dot(d, n) * n);

						m_pBall->getRigidBody()->velocity = r;
						m_pBall->getRigidBody()->velocity *= (1.0f / m_pBall->getMass());
					}
				});

				break;
			}

			if (res.collided)
			{
				//while colliding, push the object out of penetration distance
				//this is especially important for the floor
				if (!m_pBall->AtRest())
				{
					glm::vec2 surfNorm = glm::normalize(m_pBall->getPosition() - res.collisionPoint);
					//this will happen if the collision is too fast so just ignore really fast hits
					//this can be fixed by having physics system with a fixed time step that does integration a few frames ahead
					if (!glm::all(glm::isnan(surfNorm)))
					{
						glm::vec2 newPos = m_pBall->getPosition();

						//never adjust for penetration more than the radius as it will probably cause a NaN error when going too fast
						float penLim = m_pBall->getWidth();
						float maxPen = std::min(std::abs(res.penetration), penLim);

						glm::vec2 transNorm = surfNorm * maxPen;

						newPos += transNorm;

						auto diff = m_pBall->getPosition() - newPos;

						m_pBall->getTransform()->position = newPos;

						//always add some friction if colliding with a wall
						glm::vec2 surfDir(surfNorm.y, -surfNorm.x);
						surfDir *= m_wallFriction;

						//a bit of a hack... don't adjust the velocity that isn't on the plane
						if (surfDir.x != 0.0f)
						{
							m_pBall->getRigidBody()->velocity.x *= abs(surfDir.x);
						}
						if (surfDir.y != 0.0f)
						{
							m_pBall->getRigidBody()->velocity.y *= abs(surfDir.y);
						}
					}
					else
					{
						//something went wrong! reset the ball?
					}
				}
			}

			m_frameCount++;
		}

		CollisionResult playerResult = CollisionManager::circleAABBCheck(m_pBall, m_pPlayerPaddle, [this](CollisionResult result) -> void
		{
			//get a straight line between the ball and the collision surface 
			//this is used to reflect velocity around
			glm::vec2 n = glm::normalize(m_pBall->getPosition() - result.collisionPoint);
			glm::vec2 d = m_pBall->getVelocity();
			glm::vec2 r = d - 2.0f * (glm::dot(d, n) * n);

			//now we have the direction figure out the speed
			glm::vec2 rNorm = glm::normalize(r); //reduce to unit vector so we can multiply
			glm::vec2 newVel = r + m_pPlayerPaddle->getVelocity();

			newVel *= (1.0f / m_pBall->getMass()) + (1.0f / m_pPlayerPaddle->getMass());

			m_pBall->getRigidBody()->velocity = newVel;

		}, true /*this object has a centered origin*/);

		if (playerResult.collided)
		{
			glm::vec2 surfNorm = glm::normalize(m_pBall->getPosition() - playerResult.collisionPoint);
			//this will happen if the collision is too fast so just ignore really fast hits
			//this can be fixed by having physics system with a fixed time step that does integration a few frames ahead
			if (!glm::all(glm::isnan(surfNorm)))
			{
				glm::vec2 newPos = m_pBall->getPosition();

				//never adjust for penetration more than a factor of the radius as it will probably cause a NaN error
				float penLim = m_pBall->getWidth() / 10.0f;
				float maxPen = std::min(std::abs(playerResult.penetration), penLim);

				glm::vec2 transNorm = surfNorm * maxPen;

				newPos += transNorm;

				auto diff = m_pBall->getPosition() - newPos;

				m_pBall->getTransform()->position = newPos;
			}
		}
	}

	//update main game items
	updateDisplayList();

	//prepare to change scenes
	if (m_willChange)
	{
		TheGame::Instance()->changeSceneState(m_nextScene);
	}
}

void PlayScene2::clean()
{
	removeAllChildren();
}

void PlayScene2::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(END_SCENE);
	}
}

void PlayScene2::start()
{
	// Load background
	TextureManager::Instance()->load("../Assets/backgrounds/space.png", "space");

	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_width = TheGame::Instance()->getWindowWidth();
	m_height = TheGame::Instance()->getWindowHeight();

	m_playZone[0] = { 0.0f, 0.0f };
	m_playZone[1] = { m_width, m_height };

	m_pPlayerPaddle = std::make_shared<Helicopter>();
	m_pPlayerPaddle->getRigidBody()->mass = 20.0f;

	addChild(m_pPlayerPaddle.get(), 0, std::nullopt, false);

	m_pBall = std::make_shared<Target>();
	m_pBall->getRigidBody()->mass = 2.0f;
	m_pBall->Gravity = 9.81f;
	m_pBall->getTransform()->position = glm::vec2(100.0f, 100.0f);
	m_pBall->getRigidBody()->velocity = glm::vec2(20.0f, 20.0f);
	m_pBall->getRigidBody()->terminalVelocity = glm::vec2(100.0f, 100.0f);

	addChild(m_pBall.get(), 0, std::nullopt, false);

	//the game zone is sort of a dummy we can use to do collision tests with
	m_pGameZone = std::make_shared<PlayZone>(m_width, m_height);
	m_pGameZone->getTransform()->position = glm::vec2(m_width / 2.0f, m_height / 2.0f);

	//the borders
	m_pBorders[0] = std::make_shared<PlayZone>(m_width, 30.0f);
	m_pBorders[1] = std::make_shared<PlayZone>(30.0f, m_height);
	m_pBorders[2] = std::make_shared<PlayZone>(m_width, 30.0f);
	m_pBorders[3] = std::make_shared<PlayZone>(30.0f, m_height);

	//bottom border
	m_pBorders[0]->getTransform()->position = glm::vec2(0.0f, m_height - 20.0f);
	//left border
	m_pBorders[1]->getTransform()->position = glm::vec2(0.0f, 0.0f);
	//top border
	m_pBorders[2]->getTransform()->position = glm::vec2(0.0f, 0.0f);
	//right border
	m_pBorders[3]->getTransform()->position = glm::vec2(m_width - 20.0f, 0.0f);


	//// Back Button
	m_pBackButton = std::make_shared<Button>("../Assets/textures/backButton.png", "backButton", BACK_BUTTON);
	m_pBackButton->getTransform()->position = glm::vec2(100.0f, 550.0f);
	m_pBackButton->addEventListener(CLICK, [&]()-> void
	{
		m_pBackButton->setActive(false);
		m_nextScene = m_lastScene;
		m_willChange = true;
	});

	m_pBackButton->addEventListener(MOUSE_OVER, [&]()->void { m_pBackButton->setAlpha(128); });

	m_pBackButton->addEventListener(MOUSE_OUT, [&]()->void { m_pBackButton->setAlpha(255); });

	addChild(m_pBackButton.get(), 0, std::nullopt, false);

	// Next Button
	m_pNextButton = std::make_shared<Button>("../Assets/textures/nextButton.png", "nextButton", NEXT_BUTTON);
	m_pNextButton->getTransform()->position = glm::vec2(TheGame::Instance()->getWindowWidth() - 100.0f, 550.0f);
	m_pNextButton->addEventListener(CLICK, [&]()-> void
	{
		m_pNextButton->setActive(false);
		m_nextScene = START_SCENE;
		m_willChange = true;
	});

	m_pNextButton->addEventListener(MOUSE_OVER, [&]()->void { m_pNextButton->setAlpha(128); });

	m_pNextButton->addEventListener(MOUSE_OUT, [&]()->void { m_pNextButton->setAlpha(255); });

	addChild(m_pNextButton.get(), 0, std::nullopt, false);

	/* Instructions Label */
	const SDL_Color Gold = { 212,175, 55, 0 };
	m_pInstructionsLabel = std::make_shared<Label>("Press the backtick (`) character to toggle Debug View", "Consolas", 20, Gold);
	m_pInstructionsLabel->getTransform()->position = glm::vec2(Config::SCREEN_WIDTH * 0.5f, 500.0f);
	addChild(m_pInstructionsLabel.get(), 0, std::nullopt, false);
}

void PlayScene2::GUI_Function()
{
	ImGui::NewFrame();

	//draw UI in here
	//player
	ImGui::Begin("Scene control", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	ImGui::Checkbox("Draw debug shapes", &m_drawDebug);
	ImGui::Checkbox("Draw borders", &m_drawWalls);
	ImGui::Text("Player position");
	ImGui::Text("x: %f y: %f", m_pPlayerPaddle->getPosition().x, m_pPlayerPaddle->getPosition().y);
	ImGui::Text("Player velocity");
	ImGui::Text("x: % f y : % f", m_pPlayerPaddle->getVelocity().x, m_pPlayerPaddle->getVelocity().y);

	ImGui::Separator();

	ImGui::SliderFloat("Wall friction", &m_wallFriction, 0.0f, 1.0f);

	ImGui::End();

	//ball
	ImGui::Begin("Ball", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	if (ImGui::Checkbox("Circle", &m_activeBallType[CIRCLE]))
	{
		for (int i = 0; i < BALLTYPE_MAX; i++)
		{
			if (i != CIRCLE)
			{
				m_activeBallType[i] = false;
			}
		}

		m_pBall->resetImage("ball", "../Assets/textures/ball.png");
	}

	if (ImGui::Checkbox("Square", &m_activeBallType[SQUARE]))
	{
		for (int i = 0; i < BALLTYPE_MAX; i++)
		{
			if (i != SQUARE)
			{
				m_activeBallType[i] = false;
			}
		}

		m_pBall->resetImage("tile", "../Assets/textures/flatTile.png");
	}

	char* pauseStr = m_pBall->isPaused() ? "> Play" : "|| Pause";
	if (ImGui::Button(pauseStr))
	{
		m_pBall->togglePause();
	}

	if (ImGui::Button("Reset"))
	{
		m_pBall->getTransform()->position = glm::vec2(100.0f, 100.0f);
		m_pBall->getRigidBody()->velocity = glm::vec2(40.0f, 40.0f);
	}

	ImGui::Checkbox("Hit while paused", &m_canHitWhilePaused);

	ImGui::Text("Position");
	ImGui::Text("x: %f y: %f", m_pBall->getPosition().x, m_pBall->getPosition().y);

	ImGui::SliderFloat("Mass", &m_pBall->getRigidBody()->mass, 1.0f, 100.0f);

	ImGui::Separator();
	ImGui::Text("Velocity");
	ImGui::Text("x: % f y : % f", m_pBall->getVelocity().x, m_pBall->getVelocity().y);

	ImGui::End();

	ImGui::EndFrame();


	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	ImGui::StyleColorsDark();
}