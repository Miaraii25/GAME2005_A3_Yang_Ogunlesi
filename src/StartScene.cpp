#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "EventManager.h"

StartScene::StartScene()
{
	StartScene::start();
}

StartScene::StartScene(const SceneState lastScene)
	: StartScene()
{
}

StartScene::~StartScene()
= default;

void StartScene::draw()
{
	TextureManager::Instance()->draw("Spacebackground", 400, 300, 0, 255, true);
	drawDisplayList();
}

void StartScene::update()
{
	updateDisplayList();

	if (m_willChange)
	{
		TheGame::Instance()->changeSceneState(m_nextScene);
	}
}

void StartScene::clean()
{
	removeAllChildren();
}

void StartScene::handleEvents()
{
	EventManager::Instance().update();

	// Keyboard Events
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(PLAY_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(PLAY_SCENE_2);
	}
}

void StartScene::start()
{
	TextureManager::Instance()->load("../Assets/textures/Spacebackground.png", "Spacebackground");

	const SDL_Color Orange = { 255, 140, 0, 0 };
	m_pStartLabel = new Label("START SCENE", "Dock51", 80, Orange, glm::vec2(400.0f, 80.0f));
	m_pStartLabel->setParent(this);
	addChild(m_pStartLabel);

	m_pNameLabel = new Label("MINGKUN YANG  (ID:101235517)", "PinkChicken-Regular", 40, Orange, glm::vec2(400.0f, 160.0f));
	m_pNameLabel->setParent(this);
	addChild(m_pNameLabel);

	m_pNameLabel2 = new Label(" MARIAM OGUNLESI  (ID:101285729)", "PinkChicken-Regular", 40, Orange, glm::vec2(400.0f, 210.0f));
	m_pNameLabel2->setParent(this);
	addChild(m_pNameLabel2);

	m_pNameLabel3 = new Label(" AROSHABEL MOORE (ID:101156682)", "PinkChicken-Regular", 40, Orange, glm::vec2(400.0f, 260.0f));
	m_pNameLabel3->setParent(this);
	addChild(m_pNameLabel3);

	m_pScene1Label = new Label("SCENE ONE", "Thorsley", 16, Orange, glm::vec2(400.0f, 310.0f));
	m_pScene1Label->setParent(this);
	addChild(m_pScene1Label);

	m_pScene2Label = new Label("SCENE TWO", "Thorsley", 16, Orange, glm::vec2(400.0f, 460.0f));
	m_pScene2Label->setParent(this);
	addChild(m_pScene2Label);

	


	//m_pShip = new Ship();
	//m_pShip->getTransform()->position = glm::vec2(400.0f, 300.0f); 
	//addChild(m_pShip); 

	// Start Button
	m_pStartLevel1Button = new Button();
	m_pStartLevel1Button->getTransform()->position = glm::vec2(400.0f, 370.0f);

	m_pStartLevel1Button->addEventListener(CLICK, [&]()-> void
	{
		m_pStartLevel1Button->setActive(false);
		m_nextScene = PLAY_SCENE;
		m_willChange = true;
	});

	m_pStartLevel1Button->addEventListener(MOUSE_OVER, [&]()->void
	{
		m_pStartLevel1Button->setAlpha(128);
	});

	m_pStartLevel1Button->addEventListener(MOUSE_OUT, [&]()->void
	{
		m_pStartLevel1Button->setAlpha(255);
	});
	addChild(m_pStartLevel1Button);

	//level 2 button
	m_pStartLevel2Button = new Button();
	m_pStartLevel2Button->getTransform()->position = glm::vec2(400.0f, 520.0f);

	m_pStartLevel2Button->addEventListener(CLICK, [&]()-> void
	{
		m_pStartLevel2Button->setActive(false);
		TheGame::Instance()->changeSceneState(PLAY_SCENE_2);
	});

	m_pStartLevel2Button->addEventListener(MOUSE_OVER, [&]()->void
	{
		m_pStartLevel2Button->setAlpha(128);
	});

	m_pStartLevel2Button->addEventListener(MOUSE_OUT, [&]()->void
	{
		m_pStartLevel2Button->setAlpha(255);
	});
	addChild(m_pStartLevel2Button);
}

