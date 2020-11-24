#pragma once
#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include "Transform.h"
#include "RigidBody.h"
#include <string>

// enums
#include "GameObjectType.h"


class GameObject {
public:
	GameObject();
	GameObject(const GameObject& other);
	virtual ~GameObject();

	// Draw the object
	virtual void draw() = 0;

	// Update the object
	virtual void update() = 0;

	// remove anything that needs to be deleted
	virtual void clean() = 0;

	// getters for common variables
	Transform* getTransform();
	glm::vec2 getPosition() const	{ return m_transform.position; }
	glm::vec2 getRotation() const	{ return m_transform.rotation; }
	glm::vec2 getScale() const		{ return m_transform.scale; }

	// getters and setters for physics properties
	RigidBody* getRigidBody();
	glm::vec2 getTerminalVelocity() const { return m_rigidBody.terminalVelocity; }
	glm::vec2 getVelocity() const	{ return m_rigidBody.velocity; }
	float getMass() const			{ return m_rigidBody.mass; }
	glm::vec2 getAcceleration()		{ return m_rigidBody.acceleration; }

	// getters and setters for game object properties
	int getWidth() const;
	int getHeight() const;
	void setWidth(int new_width);
	void setHeight(int new_height);
	GameObjectType getType() const;
	void setType(GameObjectType new_type);

	void setEnabled(bool state);
	bool isEnabled() const;

	void togglePause() { m_paused = !m_paused; }
	void setPaused(bool paused) { m_paused = paused; }
	bool isPaused() const { return m_paused; }

protected:
	// transform component
	Transform m_transform;
	
	// rigid body component
	RigidBody m_rigidBody;

	// size variables
	int m_width;
	int m_height;
	GameObjectType m_type;

	bool m_enabled;
	bool m_paused;
};

#endif /* defined (__GAME_OBJECT__) */