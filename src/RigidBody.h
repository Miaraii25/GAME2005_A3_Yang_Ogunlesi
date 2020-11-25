#pragma once
#ifndef __RIGID_BODY__
#define __RIGID_BODY__
#include <glm/vec2.hpp>
#include <map>
#include <memory>

struct RigidBody
{
	float mass;
	glm::vec2 velocity;
	glm::vec2 terminalVelocity; //maximum speed we can fall under gravity
	glm::vec2 acceleration;
	std::map<RigidBody*, bool> colliders;

	bool isColliding()
	{
		for (auto collider : colliders)
		{
			if (collider.second)
				return true;
		}

		return false;
	}
};

#endif /* defined (__RIGID_BODY__) */