#pragma once
#ifndef __START_SCENE__
#define __START_SCENE__

#include "Scene.h"
#include "Label.h"
#include "ship.h"
#include "Button.h"

class StartScene final : public Scene
{
public:
	StartScene();
	StartScene(const SceneState lastScene);
	~StartScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
	
	Label* m_pStartLabel{};
	Label* m_pNameLabel{};
	Label* m_pNameLabel2{};
	Label* m_pNameLabel3{};

	Label* m_pScene1Label{};
	Label* m_pScene2Label{};

	//Ship* m_pShip{};

	Button* m_pStartLevel1Button;
	Button* m_pStartLevel2Button;
};

#endif /* defined (__START_SCENE__) */