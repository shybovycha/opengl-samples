#include "Application.h"

#pragma comment(lib, "irrlicht.lib")

int main() {
    std::unique_ptr<Application> app = std::make_unique<Application>();

    app->run();

    return 0;
}
