#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "Util.h"
#include <algorithm>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::PlayScene(SceneState lastScene)
	: PlayScene()
{
	m_lastScene = lastScene;
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	TextureManager::Instance()->draw("playscene", 400, 300, 0, 255, true);

	//draw spawn zone
	if (m_showSpawnZone)
	{
		glm::vec2 diff = m_bulletSpawnZone[1] - m_bulletSpawnZone[0];
		Util::DrawRect(m_bulletSpawnZone[0], diff.x, diff.y, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	}

	if (m_drawPlayerCollision)
	{
		glm::vec2 origin = m_pPlayer->getPosition() - glm::vec2(m_pPlayer->getWidth()+2, m_pPlayer->getHeight()+2)/2.0f;
		Util::DrawRect(origin, m_pPlayer->getWidth() + 1, m_pPlayer->getHeight() + 1);
	}

	if (m_drawBulletCollision)
	{
		for (auto b : m_bulletRain->getBullets())
		{
			Util::DrawCircle(b->getPosition(), b->getWidth()*0.5f + 1);
		}
	}

	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);

	if (EventManager::Instance().isIMGUIActive())
	{
		GUI_Function();
	}
}

void PlayScene::update()
{
	//physics checks first

	for (auto bullet : m_bulletRain->getBullets())
	{
		if (CollisionManager::squaredRadiusCheck(bullet, m_pPlayer, [](CollisionResult) -> void { SoundManager::Instance().playSound("thunder"); }))
		{
		}
	}

	glm::vec2 oldPlayerPos = m_pPlayer->getPosition();

	updateDisplayList();

	//if the player is outside the play area move them back in
	glm::vec2 disp = m_playZone[1] - m_playZone[0];
	if (!CollisionManager::pointRectCheck(m_pPlayer->getPosition(), m_playZone[0], disp.x, disp.y))
	{
		m_pPlayer->getTransform()->position = oldPlayerPos;
	}

	if (m_willChange)
	{
		TheGame::Instance()->changeSceneState(m_nextScene);
	}
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	// handle player movement with GameController
	if (SDL_NumJoysticks() > 0)
	{
		if (EventManager::Instance().getGameController(0) != nullptr)
		{
			const auto deadZone = 10000;
			if (EventManager::Instance().getGameController(0)->LEFT_STICK_X > deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
				m_playerFacingRight = true;
			}
			else if (EventManager::Instance().getGameController(0)->LEFT_STICK_X < -deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
				m_playerFacingRight = false;
			}
			else
			{
				if (m_playerFacingRight)
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
				}
				else
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
				}
			}
		}
	}


	// handle player movement if no Game Controllers found
	if (SDL_NumJoysticks() < 1)
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
			m_playerFacingRight = false;
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
			}
		}
	}
	

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

void PlayScene::start()
{
	// Load background
	TextureManager::Instance()->load("../Assets/backgrounds/playscene.png", "playscene");

	// Set GUI Title
	m_guiTitle = "Play Scene";
	m_frameCount = 0;

	//set up the bullet rain
	Target templateTarget;
	templateTarget.getRigidBody()->mass = 12.8f;
	templateTarget.Gravity = 9.81f;

	m_width = TheGame::Instance()->getWindowWidth();
	m_height = TheGame::Instance()->getWindowHeight();

	m_bulletSpawnZone[0] = { 0.0f, 0.0f };
	m_bulletSpawnZone[1] = { m_width, m_height / 8 };
	
	m_playZone[0] = { 0.0f, 0.0f };
	m_playZone[1] = { m_width, m_height };

	m_bulletTerminalVelocity = { 0.0f, 200.0f };
	m_bulletRain = std::make_shared<BulletRain>(10, &templateTarget, glm::vec2(0.0f, 0.0f), m_bulletSpawnZone, m_playZone, true);
	m_bulletRain->setTerminalVelocity(m_bulletTerminalVelocity);

	for (auto bullet : m_bulletRain->getBullets())
	{
		DisplayObject* d = static_cast<DisplayObject*>(bullet.get());
		addChild(d, 0, std::nullopt, false);
	}

	DisplayObject* d = static_cast<DisplayObject*>(m_bulletRain.get());
	addChild(d, 0, std::nullopt, false);

	// Player Sprite
	m_pPlayer = std::make_shared<Player>();
	addChild(m_pPlayer.get(), 0, std::nullopt, false);
	m_playerFacingRight = true;

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
		m_nextScene = PLAY_SCENE_2;
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

void PlayScene::GUI_Function() 
{
	// Always open with a NewFrame
	ImGui::NewFrame();
		
	ImGui::Begin("Scene Control", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	ImGui::Separator();

	ImGui::Separator();

	ImGui::End();

	ImGui::End();

	ImGui::EndFrame();

	// Don't Remove this
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	ImGui::StyleColorsDark();
}
