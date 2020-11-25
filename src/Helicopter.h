#ifndef __HELICOPTER__
#define __HELICOPTER__

#include "DisplayObject.h"
#include <queue>

class Helicopter final : public DisplayObject 
{
public:
	Helicopter();
	~Helicopter();

	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	void addFlightPath(std::queue<glm::vec2> waypoints);
	void addToFlightPath(glm::vec2 waypoint);

private:
	std::queue<glm::vec2> m_flightPath;

	glm::vec2 m_maxAcceleration;
	glm::vec2 m_maxSpeed;
};

#endif