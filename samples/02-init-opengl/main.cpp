#include <fstream>
#include <iostream>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
#include <globjects/Shader.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/vec2.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

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

    sf::Window window(videoMode, "Hello OpenGL!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    auto cornerBuffer = globjects::Buffer::create();

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling vertex shader...";

    auto vertexProgram = globjects::Program::create();
    auto vertexShaderSource = globjects::Shader::sourceFromFile("media/vertex.glsl");
    auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexShaderSource.get());
    auto vertexShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling fragment shader...";

    auto fragmentProgram = globjects::Program::create();
    auto fragmentShaderSource = globjects::Shader::sourceFromFile("media/fragment.glsl");
    auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentShaderSource.get());
    auto fragmentShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking shader programs...";

    vertexProgram->attach(vertexShader.get());
    fragmentProgram->attach(fragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating rendering pipeline...";

    auto programPipeline = globjects::ProgramPipeline::create();

    programPipeline->useStages(vertexProgram.get(), gl::GL_VERTEX_SHADER_BIT);
    programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating VAO...";

    auto vao = globjects::VertexArray::create();

    cornerBuffer->setData(
        std::array<glm::vec2, 4> {
            { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1) } },
        static_cast<gl::GLenum>(GL_STATIC_DRAW));

    vao->binding(0)->setAttribute(0);
    vao->binding(0)->setBuffer(cornerBuffer.get(), 0, sizeof(glm::vec2));
    vao->binding(0)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT));
    vao->enable(0);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    while (window.isOpen())
    {
        sf::Event event {};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

        programPipeline->use();
        vao->drawArrays(static_cast<gl::GLenum>(GL_TRIANGLE_STRIP), 0, 4);

        window.display();
    }

    return 0;
}
