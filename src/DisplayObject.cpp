#include "DisplayObject.h"

DisplayObject::DisplayObject()
= default;

DisplayObject::DisplayObject(const DisplayObject & other)
	: GameObject(other)
{
	m_layerIndex = other.m_layerIndex;
	m_layerOrderIndex = other.m_layerOrderIndex;
	m_pParentScene = other.m_pParentScene;

	m_textureName = other.m_textureName;
	m_texturePath = other.m_texturePath;
}

DisplayObject::~DisplayObject()
= default;

Scene * DisplayObject::getParent() const
{
	return m_pParentScene;
}

void DisplayObject::setParent(Scene * parent)
{
	m_pParentScene = parent;
}

uint32_t DisplayObject::getLayerIndex() const
{
	return m_layerIndex;
}
uint32_t DisplayObject::getLayerOrderIndex() const
{
	return m_layerOrderIndex;
}

void DisplayObject::setLayerIndex(const uint32_t new_index, const uint32_t new_order)
{
	m_layerIndex = new_index;
	m_layerOrderIndex = new_order;
}
