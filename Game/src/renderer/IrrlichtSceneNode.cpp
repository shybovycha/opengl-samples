#include "IrrlichtSceneNode.h"

IrrlichtSceneNode::IrrlichtSceneNode(irr::scene::ISceneNode* _node)
        : node(_node),
        SceneNode(Vector3<double>(_node->getPosition().X, _node->getPosition().Y, _node->getPosition().Z))
{
}

Vector3<double> IrrlichtSceneNode::getPosition() const
{
    const auto nodePosition = node->getPosition();

    return Vector3<double>(nodePosition.X, nodePosition.Y, nodePosition.Z);
}

irr::scene::ISceneNode* IrrlichtSceneNode::getNode() const
{
    return node;
}

void IrrlichtSceneNode::remove()
{
    if (node == nullptr)
    {
        return;
    }

    node->remove();
}

void IrrlichtSceneNode::setVisible(bool _isVisible)
{
    SceneNode::setVisible(_isVisible);

    node->setVisible(_isVisible);
}
