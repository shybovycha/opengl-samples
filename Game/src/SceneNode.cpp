#include "SceneNode.h"

SceneNode::SceneNode() : initialPosition(Vector3<double>()), visible(true) {}

SceneNode::SceneNode(const Vector3<double>& _initialPosition) : initialPosition(_initialPosition), visible(true) {}

void SceneNode::setVisible(bool _isVisible)
{
    visible = _isVisible;
}

bool SceneNode::isVisible() const
{
    return visible;
}
