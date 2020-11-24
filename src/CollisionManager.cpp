#include "CollisionManager.h"
#include "Util.h"
#include <algorithm>


int CollisionManager::squaredDistance(const glm::vec2 p1, const glm::vec2 p2)
{
	int diffOfXs = p2.x - p1.x;
	int diffOfYs = p2.y - p1.y;
	auto result = diffOfXs * diffOfXs + diffOfYs * diffOfYs;

	return result;
}

void CollisionManager::onCollision(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter, CollisionResult result)
{
	if (object1->getRigidBody()->colliders[object2->getRigidBody()] == false)
	{
		object2->getRigidBody()->colliders[object1->getRigidBody()] = true;
		object1->getRigidBody()->colliders[object2->getRigidBody()] = true;

		if (onEnter)
		{
			onEnter(result);
		}
	}
}

bool CollisionManager::squaredRadiusCheck(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter)
{
	glm::vec2 P1 = object1->getTransform()->position;
	glm::vec2 P2 = object2->getTransform()->position;
	int halfHeights = (object1->getHeight() + object2->getHeight()) * 0.5f;

	if (CollisionManager::squaredDistance(P1, P2) < (halfHeights * halfHeights)) 
	{
		onCollision(object1, object2, onEnter);

		return true;
	}
	else
	{
		object2->getRigidBody()->colliders[object1->getRigidBody()] = false;
		object1->getRigidBody()->colliders[object2->getRigidBody()] = false;
	}

	return false;
}

CollisionResult CollisionManager::AABBCheck(std::shared_ptr<GameObject> object1, std::shared_ptr<GameObject> object2, const std::function<void(CollisionResult)>& onEnter)
{
	// prepare relevant variables
	CollisionResult res;

	auto p1 = object1->getTransform()->position;
	auto p2 = object2->getTransform()->position;
	auto p1HalfExt = glm::vec2(object1->getWidth()/2.0f, object1->getHeight()/2.0f);
	float p1Width = object1->getWidth();
	float p1Height = object1->getHeight();
	float p2Width = object2->getWidth();
	float p2Height = object2->getHeight();

	if (
		p1.x < p2.x + p2Width &&
		p1.x + p1Width > p2.x&&
		p1.y < p2.y + p2Height &&
		p1.y + p1Height > p2.y
		)
	{		
		glm::vec2 diff = p1 - p2;
		glm::vec2 clamped = glm::clamp(diff, -p1HalfExt, p1HalfExt);
		glm::vec2 closest = p2 + clamped;
		diff = closest - p1;

		res.collided = true;
		res.surfaceNormal = glm::vec2(clamped.y, -clamped.x);
		res.penetration = p1Width - glm::length(diff);
		res.collisionPoint = closest;

		onCollision(object1, object2, onEnter, res);
	}
	else
	{
		res.collided = false;

		object2->getRigidBody()->colliders[object1->getRigidBody()] = false;
		object1->getRigidBody()->colliders[object2->getRigidBody()] = false;
	}

	return res;
}

bool CollisionManager::lineLineCheck(const glm::vec2 line1_start, const glm::vec2 line1_end, const glm::vec2 line2_start, const glm::vec2 line2_end)
{
	auto x1 = line1_start.x;
	auto x2 = line1_end.x;
	auto x3 = line2_start.x;
	auto x4 = line2_end.x;
	auto y1 = line1_start.y;
	auto y2 = line1_end.y;
	auto y3 = line2_start.y;
	auto y4 = line2_end.y;

	// calculate the distance to intersection point
	auto uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	auto uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{
		return true;
	}

	return false;
}

bool CollisionManager::lineRectCheck(const glm::vec2 line1_start, const glm::vec2 line1_end, const glm::vec2 rec_start, const float rect_width, const float rect_height)
{
	auto x1 = line1_start.x;
	auto x2 = line1_end.x;
	auto y1 = line1_start.y;
	auto y2 = line1_end.y;
	auto rx = rec_start.x;
	auto ry = rec_start.y;
	auto rw = rect_width;
	auto rh = rect_height;

	// check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below
	auto left = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry), glm::vec2(rx, ry + rh));
	auto right = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx + rw, ry), glm::vec2(rx + rw, ry + rh));
	auto top = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry), glm::vec2(rx + rw, ry));
	auto bottom = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry + rh), glm::vec2(rx + rw, ry + rh));

	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom) {
		return true;
	}

	return false;
}

int CollisionManager::minSquaredDistanceLineLine(glm::vec2 line1_start, glm::vec2 line1_end, glm::vec2 line2_start, glm::vec2 line2_end)
{
	auto u = line1_end - line1_start;
	auto v = line2_end - line2_start;
	auto w = line1_start - line2_start;
	auto a = Util::dot(u, u);         // always >= 0
	auto b = Util::dot(u, v);
	auto c = Util::dot(v, v);         // always >= 0
	auto d = Util::dot(u, w);
	auto e = Util::dot(v, w);
	auto D = a * c - b * b;					// always >= 0
	float sc, tc;

	// compute the line parameters of the two closest points
	if (D < Util::EPSILON) {					// the lines are almost parallel
		sc = 0.0;
		tc = (b > c ? d / b : e / c);			// use the largest denominator
	}
	else {
		sc = (b * e - c * d) / D;
		tc = (a * e - b * d) / D;
	}

	// get the difference of the two closest points
	auto dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)

	auto norm = Util::dot(dP, dP);
	return norm;
}

bool CollisionManager::lineAABBCheck(std::shared_ptr<GameObject> object1, const glm::vec2& direction, std::shared_ptr<GameObject>object2, const std::function<void(CollisionResult)>& onEnter)
{
	auto lineStart = object1->getTransform()->position;
	auto lineEnd = object1->getTransform()->position + direction * 100.0f;
	// aabb
	auto boxWidth = object2->getWidth();
	int halfBoxWidth = boxWidth * 0.5f;
	auto boxHeight = object2->getHeight();
	int halfBoxHeight = boxHeight * 0.5f;
	auto boxStart = object2->getTransform()->position - glm::vec2(halfBoxWidth, halfBoxHeight);

	if (lineRectCheck(lineStart, lineEnd, boxStart, boxWidth, boxHeight))
	{
		onCollision(object1, object2, onEnter);
		return true;
	}
	else
	{
		object2->getRigidBody()->colliders[object1->getRigidBody()] = false;
		object1->getRigidBody()->colliders[object2->getRigidBody()] = false;
	}

	return false;
}

int CollisionManager::circleAABBsquaredDistance(const glm::vec2 circle_centre, int circle_radius, const glm::vec2 box_start, const int box_width, const int box_height)
{
	auto dx = std::max(box_start.x - circle_centre.x, 0.0f);
	dx = std::max(dx, circle_centre.x - (box_start.x + box_width));
	auto dy = std::max(box_start.y - circle_centre.y, 0.0f);
	dy = std::max(dy, circle_centre.y - (box_start.y + box_height));

	return (dx * dx) + (dy * dy);
}

CollisionResult CollisionManager::circleAABBCheck(std::shared_ptr<GameObject> circle, std::shared_ptr<GameObject> aabb, const std::function<void(CollisionResult)>& onEnter, bool centeredOrigin)
{
	CollisionResult result;
	float circleRadius = std::max(circle->getWidth() * 0.5f, circle->getHeight() * 0.5f);
	glm::vec2 circleCenter = circle->getPosition();
	glm::vec2 halfExt(aabb->getWidth() / 2.0f, aabb->getHeight() / 2.0f);
	glm::vec2 aabbCenter = aabb->getPosition() + halfExt;
	if (centeredOrigin) aabbCenter -= halfExt;

	glm::vec2 diff = circleCenter - aabbCenter;
	glm::vec2 clamped = glm::clamp(diff, -halfExt, halfExt);
	glm::vec2 closest = aabbCenter + clamped;
	diff = closest - circleCenter;

	if (glm::length(diff) < circleRadius)
	{
		result.collided = true;
		result.surfaceNormal = glm::vec2(clamped.y, -clamped.x);
		result.penetration = circleRadius - glm::length(diff);
		result.collisionPoint = closest;

		onCollision(circle, aabb, onEnter, result);
	}
	else
	{
		result.collided = false;
		result.surfaceNormal = glm::vec2(0.0f, 0.0f);
		result.penetration = 0.0f;
		result.collisionPoint = glm::vec2(0.0f, 0.0f);
		circle->getRigidBody()->colliders[aabb->getRigidBody()] = false;
		aabb->getRigidBody()->colliders[circle->getRigidBody()] = false;
	}

	return result;
}

bool CollisionManager::pointRectCheck(const glm::vec2 point, const glm::vec2 rect_start, const float rect_width, const float rect_height)
{
	float topLeftX = rect_start.x;
	float topLeftY = rect_start.y;
	auto width = rect_width;
	auto height = rect_height;

	if (point.x > topLeftX&&
		point.x < topLeftX + width &&
		point.y > topLeftY&&
		point.y < topLeftY + height)
	{
		return true;
	}
	return false;
}


CollisionManager::CollisionManager()
= default;


CollisionManager::~CollisionManager()
= default;
