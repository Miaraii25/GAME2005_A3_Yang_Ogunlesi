#ifndef __BULLET_RAIN__
#define __BULLET_RAIN__

#include <memory>
#include <vector>
#include "DisplayObject.h"

class BulletRain : public DisplayObject
{
public:
	BulletRain();
	
	BulletRain(int numBullets, GameObject* templateObject, glm::vec2 initialVelocity, glm::vec2 spawnZone[2], glm::vec2 playZone[2], bool obeyGravity);

	~BulletRain();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	std::vector<std::shared_ptr<GameObject>> getBullets() const { return m_bullets; }
	void togglePause();
	void setPaused(bool paused);
	bool isPaused() const { return m_paused; }

	void setSpawnZone(glm::vec2 spawnZone[2]);

	void setTerminalVelocity(const glm::vec2& terminalVelocity);

private:
	void fillBullets(int numBullets, GameObject* templateObject);
	void resetBullet(std::shared_ptr<GameObject> bullet);

	GameObject* m_templateObject;

	bool m_paused;

	int m_numBullets;				//how many bullets to spawn
	glm::vec2 m_initialVelocity;	//the initial speed to give them
	glm::vec2 m_spawnZone[2];		//area of the world to spawn bullets in
	glm::vec2 m_playZone[2];		//leaving this zone prompts a bullet to reset
	bool m_obeyGravity;				//should add gravity to the bullet velocity

	std::vector<std::shared_ptr<GameObject>> m_bullets;
};


#endif