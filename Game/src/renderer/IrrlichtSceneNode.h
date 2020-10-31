#pragma once

#include <irrlicht.h>

#include "../core/SceneNode.h"

class IrrlichtSceneNode : public SceneNode
{
public:
    IrrlichtSceneNode(irr::scene::ISceneNode* node);

    virtual Vector3<double> getPosition() const override;

    irr::scene::ISceneNode* getNode() const;

    virtual void remove() override;

    virtual void setVisible(bool isVisible) override;

private:
    irr::scene::ISceneNode* node;
};
