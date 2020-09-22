#pragma once

#include "ISceneNodeAnimator.h"

class CameraSceneNodeAnimator : public ISceneNodeAnimator {
public:
    CameraSceneNodeAnimator(
        gui::ICursorControl* cursorControl, 
        f32 rotateSpeed = -1500.f,
        f32 zoomSpeed = 200.f, 
        f32 translationSpeed = 1500.f, 
        f32 distance = 70.f
    );

    virtual ~CameraSceneNodeAnimator();

    virtual void animateNode(ISceneNode* node, u32 timeMs);

    virtual bool OnEvent(const SEvent& event);

    virtual bool isEventReceiverEnabled() const {
        return true;
    }

    // TODO: only used for built-in animators; do we need it?
    virtual ESCENE_NODE_ANIMATOR_TYPE getType() const {
        return ESNAT_CAMERA_MAYA;
    }

    virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager = 0);

    virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const;

    virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0);

    virtual f32 getMoveSpeed() const = 0;

    virtual void setMoveSpeed(f32 moveSpeed) = 0;

    virtual f32 getRotateSpeed() const = 0;

    virtual void setRotateSpeed(f32 rotateSpeed) = 0;

    virtual f32 getZoomSpeed() const = 0;

    virtual void setZoomSpeed(f32 zoomSpeed) = 0;

    virtual f32 getDistance() const = 0;

    virtual void setDistance(f32 distance) = 0;

protected:
    gui::ICursorControl* cursorControl;
};
