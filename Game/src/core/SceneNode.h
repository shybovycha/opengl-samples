#pragma once

#include "Vector3.h"

class SceneNode
{
public:
    SceneNode();

    SceneNode(const Vector3<double>& initialPosition);

    virtual Vector3<double> getPosition() const = 0;

    virtual void remove() = 0;

    virtual void setVisible(bool isVisible);

    virtual bool isVisible() const;

protected:
    Vector3<double> initialPosition;
    bool visible;
};
