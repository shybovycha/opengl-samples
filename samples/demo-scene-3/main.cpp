#include <filesystem>
#include <iostream>
#include <sstream>
#include <random>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/Uniform.h>
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

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include "ImGuiSfmlBackend.hpp"

#include "Model.hpp"
#include "SimpleParticle.hpp"
#include "UniformParticleParamsGenerator.hpp"

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

#if defined(SYSTEM_DARWIN) || defined(HIGH_DPI)
    auto videoMode = sf::VideoMode(2048, 1536);
#else
    auto videoMode = sf::VideoMode(1024, 768);
#endif

    auto window = std::make_shared<sf::Window>(videoMode, "Hello, Configurable particles!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Initializing IMGUI..." << std::endl;

    initImGui(window);

    std::cout << "[INFO] Done initializing" << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling shadow mapping vertex shader...";

    auto shadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping.vert");
    auto shadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingVertexSource.get());
    auto shadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowMappingVertexShaderTemplate.get());

    if (!shadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping fragment shader...";

    auto shadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping.frag");
    auto shadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingFragmentSource.get());
    auto shadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowMappingFragmentShaderTemplate.get());

    if (!shadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking shadow mapping shaders..." << std::endl;

    auto shadowMappingProgram = std::make_unique<globjects::Program>();
    shadowMappingProgram->attach(shadowMappingVertexShader.get(), shadowMappingFragmentShader.get());

    auto shadowMappingLightSpaceUniform = shadowMappingProgram->getUniform<glm::mat4>("lightSpaceMatrix");
    auto shadowMappingModelTransformationUniform = shadowMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering vertex shader...";

    auto shadowRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.vert");
    auto shadowRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingVertexShaderSource.get());
    auto shadowRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowRenderingVertexShaderTemplate.get());

    if (!shadowRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile shadow rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering fragment shader...";

    auto shadowRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.frag");
    auto shadowRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingFragmentShaderSource.get());
    auto shadowRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowRenderingFragmentShaderTemplate.get());

    if (!shadowRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile chicken fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking shadow rendering shader...";

    auto shadowRenderingProgram = std::make_unique<globjects::Program>();
    shadowRenderingProgram->attach(shadowRenderingVertexShader.get(), shadowRenderingFragmentShader.get());

    auto shadowRenderingModelTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("model");
    auto shadowRenderingViewTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("view");
    auto shadowRenderingProjectionTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("projection");
    auto shadowRenderingLightSpaceMatrixUniform = shadowRenderingProgram->getUniform<glm::mat4>("lightSpaceMatrix");

    auto shadowRenderingLightPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto shadowRenderingLightColorUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto shadowRenderingCameraPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

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

    auto quadScene = importer.ReadFile("media/quad.obj", 0);

    if (!quadScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto quadModel = Model::fromAiNode(quadScene, quadScene->mRootNode);

    quadModel->setTransformation(glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-5, 0, 5)), glm::vec3(10.0f, 0, 10.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0, 0)));

    sf::Image textureImage;

    if (!textureImage.loadFromFile("media/texture.jpg"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    textureImage.flipVertically();

    auto defaultTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    defaultTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    defaultTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    defaultTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(textureImage.getSize().x, textureImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr()));

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Loading particle texture...";

    sf::Image particleTextureImage;

    if (!particleTextureImage.loadFromFile("media/leaf1.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    particleTextureImage.flipVertically();

    auto particleTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    particleTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    particleTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    particleTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(particleTextureImage.getSize().x, particleTextureImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(particleTextureImage.getPixelsPtr()));

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating particle emitter...";

    auto particleModel = Model::fromAiNode(quadScene, quadScene->mRootNode);

    auto randomParticleGenerator = std::make_shared<UniformParticleParamsGenerator>();

    randomParticleGenerator->setLifetimeInterval(1.0f, 5.0f);
    randomParticleGenerator->setScaleInterval(0.1f, 0.5f);
    randomParticleGenerator->setMassInterval(0.01f, 0.1f);
    randomParticleGenerator->setRotationInterval(-180.0f, 180.0f);
    randomParticleGenerator->setPositionInterval(0.0f, 6.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    randomParticleGenerator->setVelocityInterval(0.2f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

    auto particleEmitter = std::make_shared<SimpleParticleEmitter>(randomParticleGenerator);

    auto particleAffector = std::make_shared<SimpleParticleAffector>();
    auto particleRenderer = std::make_unique<SimpleParticleRenderer>(std::move(particleModel), std::move(particleTexture));
    auto particleSystem = std::make_unique<ParticleSystem<SimpleParticle>>(
        100,
        std::move(particleEmitter),
        std::vector<std::shared_ptr<AbstractParticleAffector<SimpleParticle>>>{ particleAffector },
        std::move(particleRenderer)
    );

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing shadowMapTexture...";

    auto shadowMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    shadowMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(2048, 2048),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing frame buffer...";

    auto framebuffer = std::make_unique<globjects::Framebuffer>();
    framebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), shadowMapTexture.get());

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

    sf::Clock clock;

    char* str1 = new char[255];
    str1[0] = 0;
    float f1 = 1.e10f;
    float f2 = 0.0f;
    glm::vec3 f3(0.0f);
    float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
    std::vector<std::string> items { "item 1", "item 2", "item 3" };
    int selectedIdx = 0;
    bool checkValue = false;
    float propertyValues[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

    bool isControllingCamera = false;

    std::vector<std::string> emitterTypes = { "Circle", "Rectangle", "Sphere", "Box" };
    int selectedEmitterTypeIdx = 0;

    float particleMass[2] = { randomParticleGenerator->getMinMass(), randomParticleGenerator->getMaxMass() };
    float particleScale[2] = { randomParticleGenerator->getMinScale(), randomParticleGenerator->getMaxScale() };
    float particleRotation[2] = { randomParticleGenerator->getMinRotation(), randomParticleGenerator->getMaxRotation() };
    float particleLifetime[2] = { randomParticleGenerator->getMinLifetime(), randomParticleGenerator->getMaxLifetime() };
    float particleVelocityDir[3] = { randomParticleGenerator->getVelocityDirection().x, randomParticleGenerator->getVelocityDirection().y, randomParticleGenerator->getVelocityDirection().z };
    float particleVelocityScale[2] = { randomParticleGenerator->getMinScale(), randomParticleGenerator->getMaxScale() };
    float particlePosition[3] = { randomParticleGenerator->getOrigin().x, randomParticleGenerator->getOrigin().y, randomParticleGenerator->getOrigin().z };

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window->isOpen())
    {
        sf::Event event {};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        window->setTitle("Hello, Particles! FPS: " + std::to_string(1.0f / deltaTime));

        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
                break;
            }

            processSfmlEventWithImGui(event);
        }

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);

        if (isControllingCamera)
        {
#ifdef WIN32
            glm::vec2 mouseDelta = currentMousePos - glm::vec2((window->getSize().x / 2), (window->getSize().y / 2));
            sf::Mouse::setPosition(sf::Vector2<int>(window->getSize().x / 2, window->getSize().y / 2), *window);
#else
            glm::vec2 mouseDelta = currentMousePos - previousMousePos;
            previousMousePos = currentMousePos;
#endif

            float horizontalAngle = (mouseDelta.x / static_cast<float>(window->getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
            float verticalAngle = (mouseDelta.y / static_cast<float>(window->getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

            cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
            cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

            cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    cameraPos += cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
                }
                else
                {
                    cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    cameraPos -= cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
                }
                else
                {
                    cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
                }
                else
                {
                    cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
                }
                else
                {
                    cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                isControllingCamera = false;
            }
        }

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float) window->getSize().x / (float) window->getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 4.0f); // cameraPos;

        const float nearPlane = 0.1f;
        const float farPlane = 10.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        ::glViewport(0, 0, 2048, 2048);

        // first render pass - shadow mapping

        framebuffer->bind();

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_DEPTH_BUFFER_BIT);
        framebuffer->clearBuffer(static_cast<gl::GLenum>(GL_DEPTH), 0, glm::vec4(1.0f));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_FRONT);

        shadowMappingProgram->use();

        shadowMappingLightSpaceUniform->set(lightSpaceMatrix);

        shadowMappingModelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        // the ground plane will get culled, we don't want that
        glDisable(GL_CULL_FACE);

        shadowMappingModelTransformationUniform->set(quadModel->getTransformation());

        quadModel->bind();
        quadModel->draw();
        quadModel->unbind();

        framebuffer->unbind();

        shadowMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glViewport(0, 0, static_cast<GLsizei>(window->getSize().x), static_cast<GLsizei>(window->getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadowRenderingProgram->use();

        shadowRenderingLightPositionUniform->set(lightPosition);
        shadowRenderingLightColorUniform->set(glm::vec3(1.0, 1.0, 1.0));
        shadowRenderingCameraPositionUniform->set(cameraPos);

        shadowRenderingProjectionTransformationUniform->set(cameraProjection);
        shadowRenderingViewTransformationUniform->set(cameraView);
        shadowRenderingLightSpaceMatrixUniform->set(lightSpaceMatrix);

        // draw chicken

        shadowMapTexture->bindActive(0);

        shadowRenderingProgram->setUniform("shadowMap", 0);
        shadowRenderingProgram->setUniform("diffuseTexture", 1);

        shadowRenderingModelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        shadowRenderingModelTransformationUniform->set(quadModel->getTransformation());

        defaultTexture->bindActive(1);

        quadModel->bind();
        quadModel->draw();
        quadModel->unbind();

        defaultTexture->unbindActive(1);

        shadowMapTexture->unbindActive(0);

        shadowRenderingProgram->release();

        particleSystem->update(deltaTime);
        particleSystem->draw(cameraProjection, cameraView);

        // done rendering the frame

        ImGui::NewFrame();

        if (isControllingCamera)
        {
            auto windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            
            ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 workSize = viewport->WorkSize;
            
            const float PAD = 20.0f;

            ImVec2 windowPos { workPos.x + PAD, workPos.y + PAD };
            ImVec2 windowPosPivot { 0.0f, 0.0f };

            ImGui::SetNextWindowPos(workPos, ImGuiCond_Always, windowPosPivot);

            ImGui::SetNextWindowBgAlpha(0.45f);

            if (ImGui::Begin("Info", nullptr, windowFlags))
            {
                ImGui::Text("You are currently in the \"control camera\" mode.\nThe UI won't be accessible while in this mode.\nPress Esc to get back to \"controlling UI\" mode");
            }

            ImGui::End();
        }
        else
        {
            if (ImGui::Begin("Particle system properties"))
            {
                if (ImGui::Button("Control camera"))
                {
                    isControllingCamera = true;
                }

                if (ImGui::DragFloat2("Mass", particleMass, 0.01f, 0.0f, 1.0f))
                {
                    randomParticleGenerator->setMassInterval(particleMass[0], particleMass[1]);
                }

                if (ImGui::DragFloat2("Scale", particleScale, 0.1f, 0.0f, 100.0f))
                {
                    randomParticleGenerator->setScaleInterval(particleScale[0], particleScale[1]);
                }
                
                if (ImGui::DragFloat2("Rotation", particleRotation, 0.01f, 0.0f, 1.0f))
                {
                    randomParticleGenerator->setRotationInterval(particleRotation[0], particleRotation[1]);
                }
                
                if (ImGui::DragFloat2("Lifetime", particleLifetime, 0.1f, 0.0f, 100.0f))
                {
                    randomParticleGenerator->setLifetimeInterval(particleLifetime[0], particleLifetime[1]);
                }

                ImGui::DragFloat3("Velocity direction", particleVelocityDir, 0.01f, -0.1f, 1.0f);

                // tooltip
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("This vector will be multiplied by \"velocity scale\" value");
                    ImGui::EndTooltip();
                }

                if (ImGui::DragFloat2("Velocity scale", particleVelocityScale, 0.1f, 0.0f, 100.0f))
                {
                    randomParticleGenerator->setVelocityInterval(particleVelocityScale[0], particleVelocityScale[1], glm::vec3(particleVelocityDir[0], particleVelocityDir[1], particleVelocityDir[2]));
                }

                if (ImGui::BeginCombo("Emitter type", emitterTypes[selectedEmitterTypeIdx].c_str(), 0))
                {
                    for (int i = 0; i < emitterTypes.size(); i++)
                    {
                        const bool is_selected = (selectedEmitterTypeIdx == i);

                        if (ImGui::Selectable(emitterTypes[i].c_str(), is_selected))
                        {
                            selectedEmitterTypeIdx = i;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                if (selectedEmitterTypeIdx == 0)
                {
                    // TODO: set emitter type - sphere, circle, line, box, point, etc.
                    ImGui::DragFloat3("Initial position", particlePosition, 0.01f, -0.1f, 1.0f);
                }
            }

            ImGui::End();
        }

        renderImGui(window, clock.restart().asSeconds());

        window->display();
    }

    ImGui::DestroyContext();

    return 0;
}
