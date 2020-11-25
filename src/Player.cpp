#include "Player.h"
#include "TextureManager.h"

Player::Player() 
{
	TextureManager::Instance()->loadSpriteSheet(
		"../Assets/sprites/atlas.txt",
		"../Assets/sprites/atlas.png", 
		"spritesheet");

	setSpriteSheet(TextureManager::Instance()->getSpriteSheet("spritesheet"));
	
	// set frame width
	setWidth(53);

	// set frame height
	setHeight(58);

	m_currentAnimationState = PLAYER_IDLE_RIGHT;
	m_lastAnimState = m_currentAnimationState;
	m_accelerationRate = glm::vec2(4.0f, 30.0f);
	m_moveSpeed = glm::vec2(5.0f, 40.0f);
	m_frictionFactor = 0.8f;
	getTransform()->position = glm::vec2(400.0f, 435.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	setType(PLAYER);

	m_buildAnimations();
}

Player::~Player()
= default;

void Player::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the player according to animation state
	switch(m_currentAnimationState)
	{
	case PLAYER_IDLE_RIGHT:
		TextureManager::Instance()->playAnimation("spritesheet", getAnimation("idle"),
			x, y, 0.12f, 0, 255, true);
		break;
	case PLAYER_IDLE_LEFT:
		TextureManager::Instance()->playAnimation("spritesheet", getAnimation("idle"),
			x, y, 0.12f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PLAYER_RUN_RIGHT:
		TextureManager::Instance()->playAnimation("spritesheet", getAnimation("run"),
			x, y, 0.25f, 0, 255, true);
		break;
	case PLAYER_RUN_LEFT:
		TextureManager::Instance()->playAnimation("spritesheet", getAnimation("run"),
			x, y, 0.25f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	default:
		break;
	}
	m_lastAnimState = m_currentAnimationState;
}

void Player::update()
{
	float deltaTime = 1.0f / 60.0f;

	bool applyFriction = false;
	// move the player according to animation state
	//it is possible to adjust this to stop the player from skidding when you switch from one direction to the other
	//but skidding is fun...
	switch (m_currentAnimationState)
	{
	case PLAYER_RUN_RIGHT:
		getRigidBody()->acceleration = glm::vec2(m_accelerationRate.x, 0.0f);
		break;
	case PLAYER_RUN_LEFT:
		getRigidBody()->acceleration = glm::vec2(-m_accelerationRate.x, 0.0f);
		break;
	default:
		applyFriction = true;
		getRigidBody()->acceleration = glm::vec2();
	}

	getRigidBody()->velocity += getRigidBody()->acceleration * deltaTime;

	if (getRigidBody()->velocity.x < -0.05)
	{
		getRigidBody()->velocity.x = fmax(getRigidBody()->velocity.x, -m_moveSpeed.x);
	}
	else if (getRigidBody()->velocity.x > 0.05)
	{
		getRigidBody()->velocity.x = fmin(getRigidBody()->velocity.x, m_moveSpeed.x);
	}
	else
	{
		getRigidBody()->velocity.x = 0.0f;	//stop any micro movements
	}

	getTransform()->position += getRigidBody()->velocity;

	if (applyFriction)
	{
		getRigidBody()->velocity *= m_frictionFactor;
	}
}

void Player::clean()
{
}

void Player::setAnimationState(const PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Player::m_buildAnimations()
{
	Animation idleAnimation = Animation();

	idleAnimation.name = "idle";
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-0"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-1"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-2"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-3"));

	setAnimation(idleAnimation);

	Animation runAnimation = Animation();

	runAnimation.name = "run";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-1"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-2"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-3"));

	setAnimation(runAnimation);
}
