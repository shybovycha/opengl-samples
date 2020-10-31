#include "Application.h"

Application::Application() : device(nullptr)
{
}

void Application::run()
{
    device = irr::createDevice(
            irr::video::EDT_OPENGL,
            irr::core::dimension2d<irr::u32>(1024, 768),
            32,
            false,
            false,
            false,
            nullptr
    );

    device->setWindowCaption(L"Shoot Them! Editor");

    std::shared_ptr<ApplicationDelegate> delegate = std::make_shared<ApplicationDelegate>(device);

    std::shared_ptr<IrrlichtEventReceiver> receiver = std::make_shared<IrrlichtEventReceiver>(delegate);

    device->setEventReceiver(receiver.get());

    delegate->init();

    while (device->run())
    {
        delegate->update();
    }

    device->drop();
}
