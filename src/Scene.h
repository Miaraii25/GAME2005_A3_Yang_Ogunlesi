#pragma once
#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <optional>
#include "GameObject.h"
#include "SceneState.h"

class Scene : public GameObject
{
public:
	friend class DisplayObject;
	Scene();
	Scene(const SceneState lastScene);
	virtual ~Scene();

	// Inherited via GameObject
	virtual void draw() override = 0;
	virtual void update() override = 0;
	virtual void clean() override = 0;
	virtual void handleEvents() = 0;
	virtual void start() = 0;

	void addChild(DisplayObject* child, uint32_t layer_index = 0, std::optional<uint32_t> order_index = std::nullopt, bool canDelete = true);
	void removeChild(DisplayObject* child);
	
	void removeAllChildren();
	int numberOfChildren() const;

	void updateDisplayList();
	void drawDisplayList();

private:
	uint32_t m_nextLayerIndex = 0;
	std::vector<DisplayObject*> m_displayList;
	std::vector<DisplayObject*> m_deleteList; //objects that are safe to delete

	static bool sortObjects(DisplayObject* left, DisplayObject* right);

protected:

	SceneState m_lastScene;
	bool m_willChange;	//tells
	SceneState m_nextScene;
};

#endif /* defined (__SCENE__) */
