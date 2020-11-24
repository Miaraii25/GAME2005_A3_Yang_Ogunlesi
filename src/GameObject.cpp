#include "GameObject.h"

GameObject::GameObject():
	m_width(0), m_height(0), m_type(NONE), m_enabled(true), m_paused(false)
{
}

GameObject::GameObject(const GameObject& other)
{
	m_transform = other.m_transform;

	// rigid body component
	m_rigidBody = other.m_rigidBody;

	// size variables
	m_width = other.m_width;
	m_height = other.m_height;
	m_type = other.m_type;

	m_enabled = other.m_enabled;
	m_paused = other.m_paused;
}

GameObject::~GameObject()
= default;

Transform* GameObject::getTransform() 
{
	return &m_transform;
}

RigidBody* GameObject::getRigidBody() 
{
	return &m_rigidBody;
}

int GameObject::getWidth() const
{
	return m_width;
}

int GameObject::getHeight() const
{
	return m_height;
}

GameObjectType GameObject::getType() const
{
	return m_type;
}

void GameObject::setWidth(const int new_width)
{
	m_width = new_width;
}

void GameObject::setHeight(const int new_height)
{
	m_height = new_height;
}

void GameObject::setType(const GameObjectType new_type)
{
	m_type = new_type;
}

void GameObject::setEnabled(const bool state)
{
	m_enabled = state;
}

bool GameObject::isEnabled() const
{
	return m_enabled;
}
