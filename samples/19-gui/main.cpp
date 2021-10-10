#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
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
    settings.majorVersion = 4;
    settings.minorVersion = 6;
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

    char* str1 = new char[255];
    str1[0] = 0;
    float f1 = 1.e10f;
    float f2 = 0.0f;
    glm::vec3 f3(0.0f);
    float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
    std::vector<std::string> items{ "item 1", "item 2", "item 3" };
    int selectedIdx = 0;
    bool checkValue = false;
    float propertyValues[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

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

        ImGui::Begin("Another Window");
            ImGui::Text("Hi from another window!");
            ImGui::Button("Close Me");

            if (ImGui::CollapsingHeader("Help"))
            {
                std::ostringstream s;
                s << "You have found a checkbox: "
                    << "["
                    << (checkValue ? "x" : " ")
                    << "]";

                ImGui::Text(s.str().c_str());

                ImGui::Checkbox("Checkbox: ", &checkValue);

                if (ImGui::TreeNode("Tree"))
                {
                    if (ImGui::TreeNode("sub-node-1"))
                    {
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("sub-node-2"))
                    {
                        if (ImGui::TreeNode("sub-node-2-1"))
                        {
                            if (ImGui::TreeNode("sub-node-2-1-1"))
                            {
                                ImGui::TreePop();
                            }

                            if (ImGui::TreeNode("sub-node-2-1-2"))
                            {
                                ImGui::TreePop();
                            }

                            if (ImGui::TreeNode("sub-node-2-1-3"))
                            {
                                ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("sub-node-2-2"))
                        {
                            ImGui::TreePop();
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("sub-node-3"))
                    {
                        ImGui::TreePop();
                    }
                }
            }

            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, 255);

            ImGui::InputFloat("input float", &f1, 0.0f, 0.0f, "%f");

            ImGui::SliderFloat("slider float", &f2, 0.0f, 1.0f, "ratio = %.3f");

            ImGui::InputFloat3("input float3", &f3.x);

            ImGui::ColorEdit3("color", col2);

            if (ImGui::BeginCombo("combo 1", items[selectedIdx].c_str(), 0))
            {
                for (int n = 0; n < items.size(); n++)
                {
                    const bool is_selected = (selectedIdx == n);

                    if (ImGui::Selectable(items[n].c_str(), is_selected))
                        selectedIdx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Avocado"))
                {
                    ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Broccoli"))
                {
                    ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Cucumber"))
                {
                    ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

        ImGui::End();

        ImGui::Begin("Property editor");

            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            {
                // Iterate placeholder objects (all the same data)
                for (int i = 0; i < 4; i++)
                {
                    ImGui::PushID(i);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();
                    // ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                    // ImGui::TreeNodeEx("Field", flags, "Field_%d", i);
                    ImGui::Text("property");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputFloat("##value", &(propertyValues[i]), 1.0f);
                    //ImGui::Separator();
                    // ImGui::NextColumn();

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }

        ImGui::End();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "CTRL+O"))
                {
                }

                if (ImGui::MenuItem("Save", "CTRL+S"))
                {
                }

                if (ImGui::MenuItem("Save as", "CTRL+SHIFT+S"))
                {
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z"))
                {
                }

                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
                {
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Cut", "CTRL+X"))
                {
                }

                if (ImGui::MenuItem("Copy", "CTRL+C"))
                {
                }

                if (ImGui::MenuItem("Paste", "CTRL+V"))
                {
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

         ImGui::BulletText("Double-click on title bar to collapse window.");

        ::glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glViewport(0, 0, static_cast<GLsizei>(window->getSize().x), static_cast<GLsizei>(window->getSize().y));

        renderImGui(std::weak_ptr(window), clock.restart().asSeconds());

        window->display();
    }

    ImGui::DestroyContext();

    return 0;
}
