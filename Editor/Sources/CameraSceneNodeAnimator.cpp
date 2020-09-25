#include "CameraSceneNodeAnimator.h"

CameraSceneNodeAnimator::CameraSceneNodeAnimator(
    irr::gui::ICursorControl* _cursorControl,
    irr::f32 _rotateSpeed,
    irr::f32 _moveSpeed
) :
    irr::scene::ISceneNodeAnimator(),
    cursorControl(_cursorControl),
    rotateSpeed(_rotateSpeed),
    moveSpeed(_moveSpeed),
    mouseWheelMotion(0.f),
    currentMousePosition(irr::core::vector2di(0, 0)),
    previousMousePosition(irr::core::vector2di(0, 0)),
    previousTimestamp(0),
    isLeftMouseButtonPressed(false),
    isMiddleMouseButtonPressed(false),
    isRightMouseButtonPressed(false)
{}

irr::scene::ISceneNodeAnimator* CameraSceneNodeAnimator::createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager) {
    return new CameraSceneNodeAnimator(cursorControl, rotateSpeed, moveSpeed);
}

bool CameraSceneNodeAnimator::isEventReceiverEnabled() const {
    return true;
}

void CameraSceneNodeAnimator::animateNode(irr::scene::ISceneNode* sceneNode, irr::u32 timestamp) {
    if (!shouldAnimate(sceneNode)) {
        previousTimestamp = timestamp;
        return;
    }

    irr::scene::ICameraSceneNode* camera = reinterpret_cast<irr::scene::ICameraSceneNode*>(sceneNode);

    const irr::core::vector3df up = camera->getUpVector() / camera->getUpVector().getLength();
    const irr::core::vector3df forward = (camera->getTarget() - camera->getPosition()).normalize();
    const irr::core::vector3df right = up.crossProduct(forward).normalize();

    irr::f32 deltaTime = (timestamp - previousTimestamp) / 1000.f;

    // frames are rendering too fast
    if (deltaTime == 0.f) {
        deltaTime = 1e-3;
    }

    const irr::core::vector2di deltaMousePosition = currentMousePosition - previousMousePosition;

    irr::core::vector3df cameraRelativeRotation = forward.getHorizontalAngle();

    cameraRelativeRotation.X += deltaMousePosition.Y * rotateSpeed * deltaTime;
    cameraRelativeRotation.Y += deltaMousePosition.X * rotateSpeed * deltaTime;

    // rotating camera FPS-style
    if (isRightMouseButtonPressed && fabs(deltaMousePosition.getLength()) > 0) {
        irr::core::vector3df target(0, 0, irr::core::max_(1.f, camera->getPosition().getLength()));

        irr::core::matrix4 mat;
        mat.setRotationDegrees(irr::core::vector3df(cameraRelativeRotation.X, cameraRelativeRotation.Y, 0));
        mat.transformVect(target);

        camera->setTarget(target + camera->getPosition());

        previousMousePosition = currentMousePosition;
    }
    // horizontal and vertical camera translation
    else if (isMiddleMouseButtonPressed && fabs(deltaMousePosition.getLength()) > 0) {
        irr::core::vector3df normalizedDeltaMousePosition = irr::core::vector3df(deltaMousePosition.X, deltaMousePosition.Y, 0).normalize();

        irr::core::vector3df offset = ((right * normalizedDeltaMousePosition) + (up * normalizedDeltaMousePosition)) * moveSpeed * deltaTime;

        camera->setPosition(camera->getPosition() + offset);
        camera->setTarget(camera->getTarget() + offset);

        previousMousePosition = currentMousePosition;
    }
    // forward / backward camera translation
    else if (fabs(mouseWheelMotion) > 0) {
        irr::core::vector3df offset = forward * moveSpeed * deltaTime * mouseWheelMotion;

        camera->setPosition(camera->getPosition() + offset);
        camera->setTarget(camera->getTarget() + offset);

        mouseWheelMotion = 0;
    }

    previousTimestamp = timestamp;
}

bool CameraSceneNodeAnimator::OnEvent(const irr::SEvent& event) {
    bool result = false;

    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        switch (event.MouseInput.Event) {
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
            isLeftMouseButtonPressed = true;
            break;

        case irr::EMIE_RMOUSE_PRESSED_DOWN:
            isRightMouseButtonPressed = true;
            break;

        case irr::EMIE_MMOUSE_PRESSED_DOWN:
            isMiddleMouseButtonPressed = true;
            break;

        case irr::EMIE_LMOUSE_LEFT_UP:
            isLeftMouseButtonPressed = false;
            break;

        case irr::EMIE_RMOUSE_LEFT_UP:
            isRightMouseButtonPressed = false;
            break;

        case irr::EMIE_MMOUSE_LEFT_UP:
            isMiddleMouseButtonPressed = false;
            break;

        case irr::EMIE_MOUSE_MOVED:
            previousMousePosition = currentMousePosition;
            currentMousePosition = irr::core::vector2di(event.MouseInput.X, event.MouseInput.Y);
            break;

        case irr::EMIE_MOUSE_WHEEL:
            mouseWheelMotion = event.MouseInput.Wheel;
            break;
        }
    }

    return isCapturingMouseInput();
}

bool CameraSceneNodeAnimator::isCapturingMouseInput() const {
    return (fabs(mouseWheelMotion) > 1e-5) || (isRightMouseButtonPressed) || (isMiddleMouseButtonPressed);
}

bool CameraSceneNodeAnimator::shouldAnimate(irr::scene::ISceneNode* sceneNode) const {
    return sceneNode->getType() == irr::scene::ESNT_CAMERA;
}

irr::f32 CameraSceneNodeAnimator::getMoveSpeed() const {
    return moveSpeed;
}

void CameraSceneNodeAnimator::setMoveSpeed(irr::f32 _moveSpeed) {
    moveSpeed = _moveSpeed;
}

irr::f32 CameraSceneNodeAnimator::getRotateSpeed() const {
    return rotateSpeed;
}

void CameraSceneNodeAnimator::setRotateSpeed(irr::f32 _rotateSpeed) {
    rotateSpeed = _rotateSpeed;
}
