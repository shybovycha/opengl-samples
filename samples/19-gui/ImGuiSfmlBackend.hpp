#pragma once

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
#include <globjects/Texture.h>
#include <globjects/TextureHandle.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include <imgui.h>

#ifdef WIN32
using namespace gl;
#endif

struct ImGui_SFML_BackendData
{
    std::map<ImGuiMouseCursor, std::shared_ptr<sf::Cursor>> mouseCursors;
    std::unique_ptr<globjects::Program> shaderProgram;
    std::unique_ptr<globjects::Shader> vertexShader;
    std::unique_ptr<globjects::Shader> fragmentShader;
    globjects::Uniform<glm::mat4>* projectionMatrix;
    globjects::Uniform<gl::GLuint64>* textureUniform;
    std::unique_ptr<globjects::VertexArray> vao;
    std::unique_ptr<globjects::Buffer> vertexBuffer;
    std::unique_ptr<globjects::Buffer> indexBuffer;
};

void initImGuiContext();

void initImGuiBackendData(ImGuiIO& io);

void initImGuiKeyMappings(ImGuiIO& io);

void initImGuiCursorMapping(ImGuiIO& io);

void initImGuiFonts(ImGuiIO& io);

void initImGuiConfigurationFlags(ImGuiIO& io);

void initImGuiClipboard(ImGuiIO& io);

void initImGuiStyles(ImGuiIO& io);

void initImGuiShaders(ImGuiIO& io);

void initImGuiDisplay(ImGuiIO& io, std::shared_ptr<sf::Window> windowPtr);

bool initImGui(std::weak_ptr<sf::Window> windowPtr);

void processSfmlEventWithImGui(sf::Event& evt);

bool renderImGui(std::weak_ptr<sf::Window> windowPtr, float deltaTime);
