#pragma once
#ifndef __COLLISION_MANAGER__
#define __COLLISION_MANAGER__

// core libraries
#include <iostream>
#include <functional>

#include "GameObject.h"
#include "ship.h"
#include <GLM/gtx/norm.hpp>
#include "SoundManager.h"

struct CollisionResult
{
	bool collided;
	glm::vec2 surfaceNormal;
	float penetration;
	glm::vec2 collisionPoint;
};

class CollisionManager
{
public:
	//collisions between objects have an optional onEnter lambda that is fired only when entering
	static bool squaredRadiusCheck(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter = {});
	static CollisionResult AABBCheck(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter = {});
	static bool lineAABBCheck(std::shared_ptr<GameObject> object1, const glm::vec2& direction, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter = {});
	static CollisionResult circleAABBCheck(std::shared_ptr<GameObject> circle, std::shared_ptr<GameObject> aabb, const std::function<void(CollisionResult)>& onEnter = {}, bool centeredOrigin = false);

	static bool lineLineCheck(glm::vec2 line1_start, glm::vec2 line1_end, glm::vec2 line2_start, glm::vec2 line2_end);
	static bool lineRectCheck(glm::vec2 line1_start, glm::vec2 line1_end, glm::vec2 rec_start, float rect_width, float rect_height);
	static bool pointRectCheck(glm::vec2 point, glm::vec2 rect_start, float rect_width, float rect_height);

private:
	CollisionManager();
	~CollisionManager();

	static void onCollision(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter = {}, CollisionResult result = {});
	static int squaredDistance(glm::vec2 p1, glm::vec2 p2);
	static int minSquaredDistanceLineLine(glm::vec2 line1_start, glm::vec2 line1_end, glm::vec2 line2_start, glm::vec2 line2_end);
	static int circleAABBsquaredDistance(glm::vec2 circle_centre, int circle_radius, glm::vec2 box_start, int box_width, int box_height);
};

#endif /* defined (__COLLISION_MANAGER__) */