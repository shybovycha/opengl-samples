#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/vec2.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include "ImGuiSfmlBackend.hpp"

#ifdef WIN32
using namespace gl;
#endif

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 2;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;

#ifdef SYSTEM_DARWIN
    auto videoMode = sf::VideoMode(2048, 1536);
#else
    auto videoMode = sf::VideoMode(1024, 768);
#endif

    auto window = std::make_shared<sf::Window>(videoMode, "Hello, Dear ImGUI!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing IMGUI..." << std::endl;

    initImGui(std::weak_ptr(window));

    std::cout << "[INFO] Done initializing" << std::endl;

    auto clock = sf::Clock();

    while (window->isOpen())
    {
        sf::Event event {};

        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
                break;
            }

            processSfmlEventWithImGui(event);
        }

        ImGui::NewFrame();

        ImGui::Begin("Another Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Пріуєт from another window!");
        ImGui::Button("Close Me");
        ImGui::End();

        // ImGui::BulletText("Double-click on title bar to collapse window.");

        ::glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glViewport(0, 0, static_cast<GLsizei>(window->getSize().x), static_cast<GLsizei>(window->getSize().y));

        renderImGui(std::weak_ptr(window), clock.restart().asSeconds());

        window->display();
    }

    ImGui::DestroyContext();

    return 0;
}
