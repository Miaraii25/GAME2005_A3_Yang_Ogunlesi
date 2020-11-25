#ifndef __PLAY_SCENE_2__
#define __PLAY_SCENE_2__

#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"
#include "Target.h"
#include "Helicopter.h"

class PlayZone : public GameObject
{
public:
	PlayZone(const float width, const float height)
		: GameObject()
	{
		setWidth(width);
		setHeight(height);
	}

	~PlayZone() = default;

	// Draw the object
	void draw() override {}

	// Update the object
	void update() override {}

	// remove anything that needs to be deleted
	void clean() override {}
};

enum BallType
{
	CIRCLE,
	SQUARE,
	BALLTYPE_MAX
};

class PlayScene2 : public Scene
{
public:
	PlayScene2();
	PlayScene2(const SceneState lastScene);
	~PlayScene2();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:

	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;

	unsigned int m_frameCount;
	glm::vec2 m_mousePosition;

	glm::vec2 m_playZone[2];

	BallType m_ballType;
	bool m_activeBallType[BALLTYPE_MAX];
	std::shared_ptr<Target> m_pBall;
	std::shared_ptr<Helicopter> m_pPlayerPaddle;
	std::shared_ptr<PlayZone> m_pGameZone;
	std::shared_ptr<PlayZone> m_pBorders[4];
	
	bool m_drawDebug;
	bool m_canHitWhilePaused;
	bool m_drawWalls;

	float m_wallFriction;

	// UI Items
	std::shared_ptr<Button> m_pBackButton;
	std::shared_ptr<Button> m_pNextButton;
	std::shared_ptr<Label> m_pInstructionsLabel;
};

#endif