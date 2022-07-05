#include "common/stdafx.hpp"

#include "common/AssimpModel.hpp"

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

    sf::Window window(videoMode, "Hello, Cascade shadow mapping!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

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

    std::cout << "[INFO] Compiling shadow mapping geometry shader...";

    auto shadowMappingGeometrySource = globjects::Shader::sourceFromFile("media/shadow-mapping.geom");
    auto shadowMappingGeometryShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingGeometrySource.get());
    auto shadowMappingGeometryShader = std::make_unique<globjects::Shader>(gl::GL_GEOMETRY_SHADER, shadowMappingGeometryShaderTemplate.get());

    if (!shadowMappingGeometryShader->compile())
    {
        std::cerr << "[ERROR] Can not compile geometry shader" << std::endl;
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

    std::cout << "[INFO] Creating shadow mapping program...";

    auto shadowMappingProgram = std::make_unique<globjects::Program>();

    shadowMappingProgram->attach(shadowMappingVertexShader.get(), shadowMappingGeometryShader.get(), shadowMappingFragmentShader.get());

    auto shadowMappingModelTransformationUniform = shadowMappingProgram->getUniform<glm::mat4>("modelTransformation");
    auto shadowMappingLightViewProjectionMatrices = shadowMappingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjectionMatrix");
    auto lightViewProjectionMatricesUniform = shadowMappingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjectionMatrix");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow debugging vertex shader...";

    auto shadowDebuggingVertexSource = globjects::Shader::sourceFromFile("media/shadow-debug.vert");
    auto shadowDebuggingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowDebuggingVertexSource.get());
    auto shadowDebuggingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowDebuggingVertexShaderTemplate.get());

    if (!shadowDebuggingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow debugging fragment shader...";

    auto shadowDebuggingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-debug.frag");
    auto shadowDebuggingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowDebuggingFragmentSource.get());
    auto shadowDebuggingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowDebuggingFragmentShaderTemplate.get());

    if (!shadowDebuggingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating shadow debugging program...";

    auto shadowDebuggingProgram = std::make_unique<globjects::Program>();
    shadowDebuggingProgram->attach(shadowDebuggingVertexShader.get(), shadowDebuggingFragmentShader.get());

    auto shadowDebuggingModelTransformationUniform = shadowDebuggingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling primitive rendering vertex shader...";

    auto primitiveRenderingVertexSource = globjects::Shader::sourceFromFile("media/primitive-rendering.vert");
    auto primitiveRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(primitiveRenderingVertexSource.get());
    auto primitiveRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), primitiveRenderingVertexShaderTemplate.get());

    if (!primitiveRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling primitive rendering fragment shader...";

    auto primitiveRenderingFragmentSource = globjects::Shader::sourceFromFile("media/primitive-rendering.frag");
    auto primitiveRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(primitiveRenderingFragmentSource.get());
    auto primitiveRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), primitiveRenderingFragmentShaderTemplate.get());

    if (!primitiveRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating primitive rendering program...";

    auto primitiveRenderingProgram = std::make_unique<globjects::Program>();
    primitiveRenderingProgram->attach(primitiveRenderingVertexShader.get(), primitiveRenderingFragmentShader.get());

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

    std::cout << "[INFO] Creating shadow rendering program...";

    auto shadowRenderingProgram = std::make_unique<globjects::Program>();
    shadowRenderingProgram->attach(shadowRenderingVertexShader.get(), shadowRenderingFragmentShader.get());

    auto shadowRenderingModelTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("model");
    auto shadowRenderingViewTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("view");
    auto shadowRenderingProjectionTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("projection");

    auto shadowRenderingLightPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto shadowRenderingLightColorUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto shadowRenderingCameraPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    auto shadowRenderingLightViewProjectionsUniform = shadowRenderingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjections");
    auto shadowRenderingSplitsUniform = shadowRenderingProgram->getUniform<std::vector<float>>("splits"); // distance from camera to zFar, e.g. (far - cameraPosition.z) * splitFraction

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto chickenScene = importer.ReadFile("media/Chicken.3ds", 0);

    if (!chickenScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto chickenModel = AssimpModel::fromAiNode(chickenScene, chickenScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    chickenModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), glm::radians(-90.0f), glm::vec3(1.0f, 0, 0)));

    auto quadScene = importer.ReadFile("media/quad.obj", 0);

    if (!quadScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto quadModel = AssimpModel::fromAiNode(quadScene, quadScene->mRootNode);

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

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing shadowMapTexture...";

    const glm::vec2 shadowMapSize = glm::vec2(2048, 2048);

    auto shadowMapTexture = std::make_unique<globjects::Texture>(gl::GL_TEXTURE_2D_ARRAY);

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    shadowMapTexture->storage3D(
        4,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT32F),
        glm::vec3(shadowMapSize, 4) // this last `4` is the number of layers of a 3D texture; must be equal to the number of frustum splits we are making
    );

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing frame buffer...";

    auto framebuffer = std::make_unique<globjects::Framebuffer>();
    framebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), shadowMapTexture.get());

    framebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;

#ifdef WIN32
    const float cameraRotateSpeed = 10.0f;
#else
    const float cameraRotateSpeed = 20.0f;
#endif

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;

    glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 4.0f); // cameraPos;

    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    const glm::vec3 _lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - lightPosition); // TODO: update to some constant

    glm::mat4 cameraProjection(1.0f);
    glm::mat4 cameraView(1.0f);

    std::vector<glm::mat4> lightViewProjectionMatrices;
    std::vector<float> splitDepths;
    const std::vector<float> splits{ { 0.0f, 0.05f, 0.2f, 0.5f, 1.0f } };

    // these vertices define view frustum in screen space coordinates
    constexpr std::array<glm::vec3, 8> _cameraFrustumSliceCornerVertices{
        {
            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        }
    };

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
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

        {
            float horizontalAngle = (mouseDelta.x / static_cast<float>(window.getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
            float verticalAngle = (mouseDelta.y / static_cast<float>(window.getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

            cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
            cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

            cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    cameraPos += cameraForward * cameraMoveSpeed * 10.0f * deltaTime;
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
                    cameraPos -= cameraForward * cameraMoveSpeed * 10.0f * deltaTime;
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
                    cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * 10.0f * deltaTime;
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
                    cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * 10.0f * deltaTime;
                }
                else
                {
                    cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
                }
            }

            cameraProjection = glm::perspective(glm::radians(fov), static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y), nearPlane, farPlane);

            cameraView = glm::lookAt(
                cameraPos,
                cameraPos + cameraForward,
                cameraUp);
        }

        {
            lightViewProjectionMatrices.clear();
            splitDepths.clear();

            glm::mat4 proj = glm::inverse(cameraProjection * cameraView);

            std::array<glm::vec3, 8> _entireFrustum{};

            std::transform(
                _cameraFrustumSliceCornerVertices.begin(),
                _cameraFrustumSliceCornerVertices.end(),
                _entireFrustum.begin(),
                [proj](glm::vec3 p) {
                    glm::vec4 v = proj * glm::vec4(p, 1.0f);
                    return glm::vec3(v) / v.w;
                }
            );

            std::array<glm::vec3, 4> _frustumEdgeDirections {};

            for (auto i = 0; i < 4; ++i)
            {
                _frustumEdgeDirections[i] = glm::normalize(_entireFrustum[4 + i] - _entireFrustum[i]);
            }

            const float _depth = farPlane - nearPlane;

            for (auto splitIdx = 1; splitIdx < splits.size(); ++splitIdx)
            {
                // frustum slice vertices in world space
                std::array<glm::vec3, 8> _frustumSliceVertices;

                for (auto t = 0; t < 4; ++t)
                {
                    _frustumSliceVertices[t] = _entireFrustum[t] + _frustumEdgeDirections[t] * _depth * splits[splitIdx - 1];
                    _frustumSliceVertices[4 + t] = _entireFrustum[t] + _frustumEdgeDirections[t] * _depth * splits[splitIdx];
                }

                // TODO: also check if camera is looking towards the light

                glm::vec3 _frustumSliceCenter(0.0f);

                for (auto p : _frustumSliceVertices)
                {
                    _frustumSliceCenter += p;
                }

                _frustumSliceCenter /= 8.0f;

                glm::vec3 _frustumRadiusVector(0.0f);

                for (auto p : _frustumSliceVertices)
                {
                    auto v = p - _frustumSliceCenter;

                    if (glm::length(_frustumRadiusVector) < glm::length(v))
                    {
                        _frustumRadiusVector = v;
                    }
                }

                // calculate new light projection
                glm::vec3 _forward = glm::normalize(_lightDirection);
                glm::vec3 _right = glm::cross(_forward, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 _up(0.0f, 1.0f, 0.0f);
                glm::mat4 _lightView = glm::lookAt(_frustumSliceCenter - glm::normalize(_lightDirection) * glm::length(_frustumRadiusVector), _frustumSliceCenter, _up);

                const float _frustumRadius = glm::length(_frustumRadiusVector);

                glm::mat4 _lightProjectionViewMatrix = glm::ortho(
                    _frustumSliceCenter.x - _frustumRadius,
                    _frustumSliceCenter.x + _frustumRadius,
                    _frustumSliceCenter.y - _frustumRadius,
                    _frustumSliceCenter.y + _frustumRadius,
                    0.0f,
                    _frustumSliceCenter.z + 2.0f * _frustumRadius
                ) * _lightView;

                lightViewProjectionMatrices.push_back(_lightProjectionViewMatrix);

                splitDepths.push_back(_depth * splits[splitIdx] * 0.7f);
            }

            shadowMappingLightViewProjectionMatrices->set(lightViewProjectionMatrices);
            shadowRenderingLightViewProjectionsUniform->set(lightViewProjectionMatrices);
            shadowRenderingSplitsUniform->set(splitDepths);
        }

        ::glViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

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

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadowRenderingProgram->use();

        shadowRenderingLightPositionUniform->set(lightPosition);
        shadowRenderingLightColorUniform->set(glm::vec3(1.0, 1.0, 1.0));
        shadowRenderingCameraPositionUniform->set(cameraPos);

        shadowRenderingProjectionTransformationUniform->set(cameraProjection);
        shadowRenderingViewTransformationUniform->set(cameraView);

        // draw chicken

        shadowMapTexture->bindActive(0);

        shadowRenderingProgram->setUniform("shadowMaps", 0);
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

        // done rendering the frame

        window.display();
    }

    return 0;
}
