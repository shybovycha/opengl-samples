#pragma once

#include <memory>

#include <irrlicht.h>

#include "ApplicationDelegate.h"
#include "GUIElementId.h"

class IrrlichtEventReceiver : public irr::IEventReceiver
{
public:
    explicit IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate);

    bool OnEvent(const irr::SEvent& event) override;

private:
    std::shared_ptr<ApplicationDelegate> delegate;
};
