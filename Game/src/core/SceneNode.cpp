#include "SceneNode.h"

SceneNode::SceneNode() : initialPosition(Vector3<double>()), visible(true) {}

SceneNode::SceneNode(const Vector3<double>& _initialPosition)
    : initialPosition(_initialPosition), visible(true)
{
}

// TODO: this is a horrible idea, but overloading won't work since we are only operating on SceneNode* instances, overloading requires SceneNode* too, but the real instances are of a IrrlichtSceneNode* type
bool SceneNode::operator==(const SceneNode& other) const
{
    return this->getPosition() == other.getPosition();
}

bool SceneNode::operator!=(const SceneNode& other) const
{
    return !(*this == other);
}

void SceneNode::setVisible(bool _isVisible)
{
    visible = _isVisible;
}

bool SceneNode::isVisible() const
{
    return visible;
}
