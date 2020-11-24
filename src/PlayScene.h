#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"
#include "Target.h"
#include "BulletRain.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	PlayScene(SceneState lastScene);
	~PlayScene();

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
	bool m_showSpawnZone;
	bool m_drawPlayerCollision;
	bool m_drawBulletCollision;

	glm::vec2 m_playZone[2];
	glm::vec2 m_bulletSpawnZone[2];
	std::shared_ptr<BulletRain> m_bulletRain;

	std::shared_ptr<Player> m_pPlayer;
	bool m_playerFacingRight;

	glm::vec2 m_bulletTerminalVelocity;

	bool m_bPlayerHasLOS;

	// UI Items
	std::shared_ptr<Button> m_pBackButton;
	std::shared_ptr<Button> m_pNextButton;
	std::shared_ptr<Label> m_pInstructionsLabel;

	unsigned int m_frameCount;
};

#endif /* defined (__PLAY_SCENE__) */