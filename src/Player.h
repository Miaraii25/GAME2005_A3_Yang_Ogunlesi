#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "PlayerAnimationState.h"
#include "Sprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player();

	// Life Cycle Methods
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// setters
	void setAnimationState(PlayerAnimationState new_state);

private:
	void m_buildAnimations();
	//the friction factor is used to slow down the player. 1 = no friction 0 = total stop
	PlayerAnimationState m_lastAnimState;
	float m_frictionFactor;
	glm::vec2 m_moveSpeed;
	glm::vec2 m_accelerationRate;
	PlayerAnimationState m_currentAnimationState;
};

#endif /* defined (__PLAYER__) */