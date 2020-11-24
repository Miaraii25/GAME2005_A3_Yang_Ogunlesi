#include "Helicopter.h"
#include "TextureManager.h"

Helicopter::Helicopter()
{
	m_textureName = "helicopter";
	m_texturePath = "../Assets/textures/helicopter_single.png";
	TextureManager::Instance()->load(m_texturePath, m_textureName);

	const auto size = TextureManager::Instance()->getTextureSize(m_textureName);
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(0.0f, 0.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);

	setType(HELICOPTER);
}

Helicopter::~Helicopter() = default;


void Helicopter::draw()
{
	const auto pos = getTransform()->position;
	TextureManager::Instance()->draw("helicopter", pos.x, pos.y, 0, 255, true);
}

//TODO: proper interpolate on flight path, but this is okay for mouse movement
void Helicopter::update()
{
	float deltaTime = 1.0f / 60.0f;

	//move the helicopter along the flight path
	if (m_flightPath.size())
	{
		glm::vec2 lastPos = getPosition();
		getTransform()->position = m_flightPath.front();
		m_flightPath.pop();

		//update velocity
		getRigidBody()->velocity = (getTransform()->position - lastPos) / deltaTime;
	}
}

void Helicopter::clean()
{

}

void Helicopter::addFlightPath(std::queue<glm::vec2> waypoints)
{
	if (waypoints.size())
	{
		while (waypoints.front() != waypoints.back())
		{
			m_flightPath.push(waypoints.front());
			waypoints.pop();
		}
	}
}

void Helicopter::addToFlightPath(glm::vec2 waypoint)
{
	m_flightPath.push(waypoint);
}