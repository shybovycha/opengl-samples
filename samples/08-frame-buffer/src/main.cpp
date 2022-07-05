#include "common/stdafx.hpp"

#include "common/Model.hpp"

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

    sf::Window window(videoMode, "Hello, Frame buffer!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling model rendering vertex shader...";

    auto modelRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/model-rendering.vert");
    auto modelRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(modelRenderingVertexShaderSource.get());
    auto modelRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), modelRenderingVertexShaderTemplate.get());

    if (!modelRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile model rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling model rendering fragment shader...";

    auto modelRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/model-rendering.frag");
    auto modelRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(modelRenderingFragmentShaderSource.get());
    auto modelRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), modelRenderingFragmentShaderTemplate.get());

    if (!modelRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile chicken fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking shaders...";

    auto modelRenderingProgram = std::make_unique<globjects::Program>();
    modelRenderingProgram->attach(modelRenderingVertexShader.get(), modelRenderingFragmentShader.get());

    auto lightPositionUniform = modelRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto lightColorUniform = modelRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto ambientColorUniform = modelRenderingProgram->getUniform<glm::vec3>("ambientColor");
    auto diffuseColorUniform = modelRenderingProgram->getUniform<glm::vec3>("diffuseColor");
    auto materialSpecularUniform = modelRenderingProgram->getUniform<float>("materialSpecular");
    auto cameraPositionUniform = modelRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    auto modelTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("model");
    auto viewTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("view");
    auto projectionTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("projection");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading chicken 3D model...";

    Assimp::Importer importer;

    auto chickenScene = importer.ReadFile("media/Chicken.3ds", 0);

    if (!chickenScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto chickenModel = Model::fromAiNode(chickenScene, chickenScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    chickenModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), glm::radians(-90.0f), glm::vec3(1.0f, 0, 0)));

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading cube 3D model...";

    auto cubeScene = importer.ReadFile("media/cube.obj", 0);

    if (!cubeScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto cubeModel = Model::fromAiNode(cubeScene, cubeScene->mRootNode);

    cubeModel->setTransformation(glm::mat4(1.0f));

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers..." << std::endl;

    std::cout << "[DEBUG] Initializing depthColorTexture...";

    auto depthColorTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    depthColorTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB8),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing renderBuffer...";

    auto renderBuffer = std::make_unique<globjects::Renderbuffer>();
    renderBuffer->storage(static_cast<gl::GLenum>(GL_DEPTH24_STENCIL8), window.getSize().x, window.getSize().y);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing frameBuffer...";

    auto framebuffer = std::make_unique<globjects::Framebuffer>();
    framebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), depthColorTexture.get());
    framebuffer->attachRenderBuffer(static_cast<gl::GLenum>(GL_DEPTH_STENCIL_ATTACHMENT), renderBuffer.get());

    framebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });
    framebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    glm::vec3 lightPosition = glm::vec3(0.0f, 2.0f, 2.0f);

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
#ifdef WIN32
        if (!window.hasFocus())
        {
            continue;
        }
#endif

        sf::Event event {};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

#ifdef WIN32
        glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));
        sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
#else
        glm::vec2 mouseDelta = currentMousePos - previousMousePos;
        previousMousePos = currentMousePos;
#endif

        float horizontalAngle = (mouseDelta.x / static_cast<float>(window.getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
        float verticalAngle = (mouseDelta.y / static_cast<float>(window.getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

        cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
        cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

        cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
        }

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        glm::mat4 renderingToTextureProjection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

        glm::mat4 renderingToTextureView = glm::lookAt(glm::vec3(0.0f, 0.0f, 4.5f), glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        lightPositionUniform->set(lightPosition);
        lightColorUniform->set(glm::vec3(1, 0.5, 0.5));
        ambientColorUniform->set(glm::vec3(1.0f, 1.0f, 1.0f));
        materialSpecularUniform->set(12.0f);
        cameraPositionUniform->set(cameraPos);

        diffuseColorUniform->set(glm::vec4(1.0, 1.0, 1.0, 1.0));

        // first render pass - render depth to texture

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

        framebuffer->bind();

        framebuffer->clearBuffer(static_cast<gl::GLenum>(GL_COLOR), 0, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        framebuffer->clearBuffer(static_cast<gl::GLenum>(GL_DEPTH), 0, glm::vec4(1.0f));

        glEnable(GL_DEPTH_TEST);

        modelRenderingProgram->use();

        // draw chicken

        projectionTransformationUniform->set(renderingToTextureProjection);
        viewTransformationUniform->set(renderingToTextureView);

        modelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        modelRenderingProgram->release();

        framebuffer->unbind();

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelRenderingProgram->use();

        projectionTransformationUniform->set(cameraProjection);
        viewTransformationUniform->set(cameraView);

        modelTransformationUniform->set(cubeModel->getTransformation());

        depthColorTexture->bind();

        cubeModel->bind();
        cubeModel->draw();
        cubeModel->unbind();

        depthColorTexture->unbind();

        modelRenderingProgram->release();

        window.display();
    }

    return 0;
}
