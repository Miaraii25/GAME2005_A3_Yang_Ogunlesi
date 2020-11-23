#pragma once
#ifndef __TARGET__
#define __TARGET__

#include "DisplayObject.h"

class Target final : public DisplayObject {
public:
	Target();
	~Target();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	bool isGravityEnabled = false;

	glm::vec2 throwSpeed;
	glm::vec2 throwPosition;

	float Gravity;

	void resetImage(const std::string& textureName, const std::string& texturePath);
	bool AtRest() { return m_atRest; }
	void WakeUp() { m_atRest = true; }

private:
	void m_move();
	void m_checkBounds();
	void m_reset();

	bool m_atRest;
};


#endif /* defined (__TARGET__) */