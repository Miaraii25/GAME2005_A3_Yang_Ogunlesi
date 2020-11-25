#include "Target.h"
#include "TextureManager.h"


Target::Target()
{
	resetImage("ball", "../Assets/textures/ball.png");

	getTransform()->position = glm::vec2(0.0f, 0.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	isGravityEnabled = true;
	m_atRest = false;

	setType(TARGET);
}

Target::Target(const Target& other)
	: DisplayObject(other)
{
	isGravityEnabled = other.isGravityEnabled;

	throwSpeed = other.throwSpeed;
	throwPosition = other.throwPosition;

	getRigidBody()->mass = other.getMass();
	Gravity = other.Gravity;
	m_atRest = other.m_atRest;
}

Target::~Target()
= default;

void Target::draw()
{
	auto pos = getTransform()->position;

	// draw the target
	TextureManager::Instance()->draw(m_textureName, pos.x, pos.y, 0, 255, true);
}

void Target::update()
{
	if (!m_paused)
	{
		m_move();
		m_checkBounds();
	}
}

void Target::clean()
{
}

void Target::resetImage(const std::string& textureName, const std::string& texturePath)
{
	m_textureName = textureName;
	m_texturePath = texturePath;
	TextureManager::Instance()->load(m_texturePath, m_textureName);

	const auto size = TextureManager::Instance()->getTextureSize(m_textureName);
	setWidth(size.x);
	setHeight(size.y);

}

void Target::m_move()
{

	float deltaTime = 1.0f / 60.0f;
	glm::vec2 gravity = isGravityEnabled ? glm::vec2(0.0f, Gravity) : glm::vec2(0, 0);

	glm::vec2 rawAccel = ((getAcceleration()*deltaTime + gravity) * getMass());
	glm::vec2 accel = rawAccel * deltaTime;
	glm::vec2 newVel = getVelocity() + accel;
	if (getTerminalVelocity().y != 0.0f && newVel.y > getTerminalVelocity().y)
	{
		newVel.y = getTerminalVelocity().y;
	}

	bool isColliding = getRigidBody()->isColliding();
	if (newVel.x < 1.0f && newVel.y < 1.0f && isColliding)
	{
		m_atRest = true;
	}
	else
	{
		m_atRest = false;
	}

	if (!m_atRest)
	{
		getRigidBody()->acceleration = rawAccel;
		getRigidBody()->velocity = newVel;

		getTransform()->position += newVel * deltaTime * 1.0f;
	}
}

void Target::m_checkBounds()
{
}

void Target::m_reset()
{
}
