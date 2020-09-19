#pragma once

#include <memory>

#include "irrlicht.h"

#include "ApplicationDelegate.h"

class IrrlichtEventReceiver : public irr::IEventReceiver {
public:
    IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate);

    virtual bool OnEvent(const irr::SEvent& event);

private:
    std::shared_ptr<ApplicationDelegate> delegate;
};
