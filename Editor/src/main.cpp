#include "Application.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "irrlicht.lib")
#endif

int main()
{
    std::unique_ptr<Application> app = std::make_unique<Application>();

    app->run();

    return 0;
}
