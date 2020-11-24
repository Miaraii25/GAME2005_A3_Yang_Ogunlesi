#include "BulletRain.h"
#include "Target.h"
#include "CollisionManager.h"

BulletRain::BulletRain()
{
	setType(BULLET_RAIN);
	m_paused = false;
	m_numBullets = 0;
	m_bullets.resize(0);
	m_initialVelocity = {};
	m_obeyGravity = false;
	m_templateObject = nullptr;
}

BulletRain::BulletRain(int numBullets, GameObject* templateObject, glm::vec2 initialVelocity, glm::vec2 spawnZone[2], glm::vec2 playZone[2], bool obeyGravity)
	: BulletRain()
{
	m_numBullets = numBullets;
	m_initialVelocity = initialVelocity;
	m_obeyGravity = obeyGravity;
	m_spawnZone[0] = spawnZone[0];
	m_spawnZone[1] = spawnZone[1];
	m_playZone[0] = playZone[0];
	m_playZone[1] = playZone[1];
	m_templateObject = templateObject;

	fillBullets(m_numBullets, templateObject);
}

BulletRain::~BulletRain()
{
	m_bullets.clear();
}

void BulletRain::fillBullets(int numBullets, GameObject* templateObject)
{
	for (int i = 0; i < numBullets; i++)
	{
		switch (templateObject->getType())
		{
		case TARGET:
		{
			Target* t = static_cast<Target*>(templateObject);
			auto newBullet = std::make_shared<Target>(*t);
			resetBullet(newBullet);
			m_bullets.push_back(newBullet);
		}	break;

		default:
			break;
		}
	}
}

void BulletRain::resetBullet(std::shared_ptr<GameObject> bullet)
{
	//reset everything and randomise the starting position
	glm::vec2 diff = m_spawnZone[1] - m_spawnZone[0];
	int rX = diff.x != 0 ? rand() % abs((int)diff.x) : 1;
	int rY = diff.y != 0 ? rand() % abs((int)diff.y) : 1;
	
	bullet->getTransform()->position = glm::vec2(m_spawnZone[0].x+rX, m_spawnZone[0].y+rY);
	bullet->getTransform()->rotation = glm::vec2(m_templateObject->getRotation());
	bullet->getTransform()->scale = glm::vec2(m_templateObject->getScale());
}

void BulletRain::draw()
{
	//doesn't need to do anything
}

void BulletRain::update()
{
	// reset any bullets that are outside the playzone
	for (auto b : m_bullets)
	{
		if (!b->isPaused())
		{
			//just in case the play zone starts somewhere other than 0,0
			glm::vec2 disp = m_playZone[1] - m_playZone[0];

			if (!CollisionManager::pointRectCheck(b->getPosition(), m_playZone[0], disp.x, disp.y))
			{
				resetBullet(b);
			}
		}
	}
}

void BulletRain::clean()
{
	m_bullets.clear();
}

void BulletRain::togglePause()
{
	m_paused = !m_paused;
	for (auto b : m_bullets)
	{
		b->setPaused(m_paused);
	}
}

void BulletRain::setPaused(bool paused)
{
	m_paused = paused;
	for (auto b : m_bullets)
	{
		b->setPaused(m_paused);
	}
}

void BulletRain::setSpawnZone(glm::vec2 spawnZone[2])
{
	m_spawnZone[0] = spawnZone[0];
	m_spawnZone[1] = spawnZone[1];
}

void BulletRain::setTerminalVelocity(const glm::vec2& terminalVelocity)
{
	for (auto b : m_bullets)
	{
		b->getRigidBody()->terminalVelocity = terminalVelocity;
	}
}