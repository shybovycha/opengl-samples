#include "common/stdafx.hpp"

#include "common/AssimpModel.hpp"

struct alignas(16) PointLightData
{
    glm::vec3 lightPosition;
    float farPlane;
    std::array<glm::mat4, 6> projectionViewMatrices;
};

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

    sf::Window window(videoMode, "Hello, Point light!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    /*std::cout << "[INFO] Compiling directional shadow mapping vertex shader...";

    auto directionalShadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping-directional.vert");
    auto directionalShadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(directionalShadowMappingVertexSource.get());
    auto directionalShadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), directionalShadowMappingVertexShaderTemplate.get());

    if (!directionalShadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile directional shadow mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling directional shadow mapping fragment shader...";

    auto directionalShadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping-directional.frag");
    auto directionalShadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(directionalShadowMappingFragmentSource.get());
    auto directionalShadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), directionalShadowMappingFragmentShaderTemplate.get());

    if (!directionalShadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile directional shadow mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking directional shadow mapping shaders..." << std::endl;

    auto directionalShadowMappingProgram = std::make_unique<globjects::Program>();
    directionalShadowMappingProgram->attach(directionalShadowMappingVertexShader.get(), directionalShadowMappingFragmentShader.get());

    auto directionalShadowMappingLightSpaceUniform = directionalShadowMappingProgram->getUniform<glm::mat4>("lightSpaceMatrix");
    auto directionalShadowMappingModelTransformationUniform = directionalShadowMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling directional shadow rendering vertex shader...";

    auto directionalShadowRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering-directional.vert");
    auto directionalShadowRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(directionalShadowRenderingVertexShaderSource.get());
    auto directionalShadowRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), directionalShadowRenderingVertexShaderTemplate.get());

    if (!directionalShadowRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile directional shadow rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling directional shadow rendering fragment shader...";

    auto directionalShadowRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering-directional.frag");
    auto directionalShadowRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(directionalShadowRenderingFragmentShaderSource.get());
    auto directionalShadowRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), directionalShadowRenderingFragmentShaderTemplate.get());

    if (!directionalShadowRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile directional shadow rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking directional shadow rendering shader...";

    auto directionalShadowRenderingProgram = std::make_unique<globjects::Program>();
    directionalShadowRenderingProgram->attach(directionalShadowRenderingVertexShader.get(), directionalShadowRenderingFragmentShader.get());

    auto directionalShadowRenderingModelTransformationUniform = directionalShadowRenderingProgram->getUniform<glm::mat4>("model");
    auto directionalShadowRenderingViewTransformationUniform = directionalShadowRenderingProgram->getUniform<glm::mat4>("view");
    auto directionalShadowRenderingProjectionTransformationUniform = directionalShadowRenderingProgram->getUniform<glm::mat4>("projection");
    auto directionalShadowRenderingLightSpaceMatrixUniform = directionalShadowRenderingProgram->getUniform<glm::mat4>("lightSpaceMatrix");

    auto directionalShadowRenderingLightPositionUniform = directionalShadowRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto directionalShadowRenderingLightColorUniform = directionalShadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto directionalShadowRenderingCameraPositionUniform = directionalShadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;*/

    std::cout << "[INFO] Compiling point shadow data buffer...";

    auto pointLightDataBuffer = std::make_unique<globjects::Buffer>();

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point shadow mapping vertex shader...";

    auto pointShadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping-point.vert");
    auto pointShadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(pointShadowMappingVertexSource.get());
    auto pointShadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), pointShadowMappingVertexShaderTemplate.get());

    if (!pointShadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point shadow mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point shadow mapping geometry shader...";

    auto pointShadowMappingGeometrySource = globjects::Shader::sourceFromFile("media/shadow-mapping-point.geom");
    auto pointShadowMappingGeometryShaderTemplate = globjects::Shader::applyGlobalReplacements(pointShadowMappingGeometrySource.get());
    auto pointShadowMappingGeometryShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_GEOMETRY_SHADER), pointShadowMappingGeometryShaderTemplate.get());

    if (!pointShadowMappingGeometryShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point shadow mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point shadow mapping fragment shader...";

    auto pointShadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping-point.frag");
    auto pointShadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(pointShadowMappingFragmentSource.get());
    auto pointShadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), pointShadowMappingFragmentShaderTemplate.get());

    if (!pointShadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point shadow mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking point shadow mapping shaders..." << std::endl;

    auto pointShadowMappingProgram = std::make_unique<globjects::Program>();
    pointShadowMappingProgram->attach(pointShadowMappingVertexShader.get(), pointShadowMappingGeometryShader.get(), pointShadowMappingFragmentShader.get());

    auto pointShadowMappingModelTransformationUniform = pointShadowMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    /*std::cout << "[INFO] Compiling point skybox rendering vertex shader...";

    auto skyboxRenderingVertexSource = globjects::Shader::sourceFromFile("media/skybox.vert");
    auto skyboxRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingVertexSource.get());
    auto skyboxRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), skyboxRenderingVertexShaderTemplate.get());

    if (!skyboxRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point skybox rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point skybox rendering fragment shader...";

    auto skyboxRenderingFragmentSource = globjects::Shader::sourceFromFile("media/skybox.frag");
    auto skyboxRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingFragmentSource.get());
    auto skyboxRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), skyboxRenderingFragmentShaderTemplate.get());

    if (!skyboxRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point skybox rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking point skybox rendering shaders..." << std::endl;

    auto skyboxRenderingProgram = std::make_unique<globjects::Program>();
    skyboxRenderingProgram->attach(skyboxRenderingVertexShader.get(), skyboxRenderingFragmentShader.get());

    auto skyboxRenderingModelTransformationUniform = skyboxRenderingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;*/

    std::cout << "[INFO] Compiling point shadow rendering vertex shader...";

    auto pointShadowRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering-point.vert");
    auto pointShadowRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(pointShadowRenderingVertexShaderSource.get());
    auto pointShadowRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), pointShadowRenderingVertexShaderTemplate.get());

    if (!pointShadowRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point shadow rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point shadow rendering fragment shader...";

    auto pointShadowRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering-point.frag");
    auto pointShadowRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(pointShadowRenderingFragmentShaderSource.get());
    auto pointShadowRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), pointShadowRenderingFragmentShaderTemplate.get());

    if (!pointShadowRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point shadow rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking point shadow rendering shader...";

    auto pointShadowRenderingProgram = std::make_unique<globjects::Program>();
    pointShadowRenderingProgram->attach(pointShadowRenderingVertexShader.get(), pointShadowRenderingFragmentShader.get());

    auto pointShadowRenderingModelTransformationUniform = pointShadowRenderingProgram->getUniform<glm::mat4>("model");
    auto pointShadowRenderingViewTransformationUniform = pointShadowRenderingProgram->getUniform<glm::mat4>("view");
    auto pointShadowRenderingProjectionTransformationUniform = pointShadowRenderingProgram->getUniform<glm::mat4>("projection");

    auto pointShadowRenderingLightColorUniform = pointShadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto pointShadowRenderingCameraPositionUniform = pointShadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto houseScene = importer.ReadFile("media/house1.obj", 0);

    if (!houseScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto houseModel = AssimpModel::fromAiNode(houseScene, houseScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    houseModel->setTransformation(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), glm::vec3(0.0f, 0.75f, 0.0f)));

    auto tableScene = importer.ReadFile("media/table.obj", 0);

    if (!tableScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto tableModel = AssimpModel::fromAiNode(tableScene, tableScene->mRootNode, { "media" });

    tableModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)) , glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    auto lanternScene = importer.ReadFile("media/lantern.obj", 0);

    if (!lanternScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto lanternModel = AssimpModel::fromAiNode(lanternScene, lanternScene->mRootNode, { "media" });

    lanternModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f, 3.85f, -0.75f)), glm::vec3(0.5f)));

    // TODO: extract this to material class
    sf::Image lanternEmissionMapImage;

    if (!lanternEmissionMapImage.loadFromFile("media/lantern_emission.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternEmissionMapImage.flipVertically();

    auto lanternEmissionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternEmissionMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternEmissionMapImage.getSize().x, lanternEmissionMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternEmissionMapImage.getPixelsPtr()));

    // TODO: extract this to material class
    sf::Image lanternSpecularMapImage;

    if (!lanternSpecularMapImage.loadFromFile("media/lantern_specular.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternSpecularMapImage.flipVertically();

    auto lanternSpecularMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternSpecularMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternSpecularMapImage.getSize().x, lanternSpecularMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternSpecularMapImage.getPixelsPtr()));

    auto scrollScene = importer.ReadFile("media/scroll.obj", 0);

    if (!scrollScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto scrollModel = AssimpModel::fromAiNode(scrollScene, scrollScene->mRootNode, { "media" });

    scrollModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.85f, 0.0f)), glm::vec3(0.5f)));

    /*sf::Image textureImage;

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
        reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr()));*/

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    /*std::cout << "[DEBUG] Initializing directionalShadowMapTexture...";

    auto directionalShadowMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    directionalShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    directionalShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    directionalShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    directionalShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    directionalShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    directionalShadowMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(2048, 2048),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing directional shadow mapping frame buffer...";

    auto directionalShadowMappingFramebuffer = std::make_unique<globjects::Framebuffer>();
    directionalShadowMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), directionalShadowMapTexture.get());

    directionalShadowMappingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;*/

    std::cout << "[DEBUG] Initializing pointShadowMapTexture...";

    auto pointShadowMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    pointShadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    pointShadowMapTexture->bind();

    const auto shadowMapSize = 2048;

    for (auto i = 0; i < 6; ++i)
    {
        ::glTexImage2D(
            static_cast<::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
            0,
            GL_DEPTH_COMPONENT,
            shadowMapSize,
            shadowMapSize,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
    }

    pointShadowMapTexture->unbind();

    /*auto skybox = Skybox::builder()
        ->top("media/skybox-top.png")
        ->bottom("media/skybox-bottom.png")
        ->left("media/skybox-left.png")
        ->right("media/skybox-right.png")
        ->front("media/skybox-front.png")
        ->back("media/skybox-back.png")
        // Skybox::fromCubemap(pointShadowMapTexture.get())
            ->size(20.0f)
            ->build();*/

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing point shadow mapping frame buffer...";

    auto pointShadowMappingFramebuffer = std::make_unique<globjects::Framebuffer>();
    pointShadowMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), pointShadowMapTexture.get());

    pointShadowMappingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    // taken from lantern position
    glm::vec3 pointLightPosition = glm::vec3(-1.75f, 6.85f, -2.75f);

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 5.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
        sf::Event event{};

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

#ifdef WIN32
        if (!window.hasFocus())
        {
            continue;
        }
#endif

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

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        const float nearPlane = 0.1f;
        const float farPlane = 10.0f;

        glm::mat4 pointLightProjection = glm::perspective(glm::radians(90.0f), static_cast<float>(shadowMapSize / shadowMapSize), nearPlane, farPlane);

        std::array<glm::mat4, 6> pointLightProjectionViewMatrices{
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            pointLightProjection * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        };

        PointLightData pointLightData{ pointLightPosition, farPlane, pointLightProjectionViewMatrices };

        pointLightDataBuffer->setData(pointLightData, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

        ::glViewport(0, 0, shadowMapSize, shadowMapSize);

        // first render pass - shadow mapping

        pointShadowMappingFramebuffer->bind();

        pointLightDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 5);

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_FRONT);

        pointShadowMappingProgram->use();

        /*pointShadowMappingProgram->setUniform("lightPosition", pointLightPosition);
        pointShadowMappingProgram->setUniform("farPlane", farPlane);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[0]", pointLightProjectionViewMatrices[0]);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[1]", pointLightProjectionViewMatrices[1]);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[2]", pointLightProjectionViewMatrices[2]);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[3]", pointLightProjectionViewMatrices[3]);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[4]", pointLightProjectionViewMatrices[4]);
        pointShadowMappingProgram->setUniform("projectionViewMatrices[5]", pointLightProjectionViewMatrices[5]);*/

        pointShadowMappingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        pointShadowMappingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        pointShadowMappingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        pointShadowMappingModelTransformationUniform->set(scrollModel->getTransformation());

        // scroll model needs culling to be disabled since this is a modified plane, so...
        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        pointLightDataBuffer->unbind(GL_SHADER_STORAGE_BUFFER, 5);

        pointShadowMappingFramebuffer->unbind();

        pointShadowMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pointShadowRenderingProgram->use();

        pointLightDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 5);

        pointShadowRenderingLightColorUniform->set(glm::vec3(1.0, 1.0, 1.0));
        pointShadowRenderingCameraPositionUniform->set(cameraPos);

        pointShadowRenderingProjectionTransformationUniform->set(cameraProjection);
        pointShadowRenderingViewTransformationUniform->set(cameraView);

        pointShadowRenderingProgram->setUniform("lightPosition", pointLightPosition);
        pointShadowRenderingProgram->setUniform("farPlane", farPlane);

        // draw the scene

        pointShadowMapTexture->bindActive(0);

        pointShadowRenderingProgram->setUniform("shadowMap", 0);
        pointShadowRenderingProgram->setUniform("diffuseTexture", 1);

        pointShadowRenderingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        pointShadowRenderingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        pointShadowRenderingModelTransformationUniform->set(lanternModel->getTransformation());

        pointShadowRenderingProgram->setUniform("emissionColor", glm::vec3(0.807f, 0.671f, 0.175f));

        lanternSpecularMapTexture->bindActive(2);
        lanternEmissionMapTexture->bindActive(3);

        pointShadowRenderingProgram->setUniform("specularMapTexture", 2);
        pointShadowRenderingProgram->setUniform("emissionMapTexture", 3);

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        lanternSpecularMapTexture->unbindActive(2);
        lanternEmissionMapTexture->unbindActive(3);

        pointShadowRenderingModelTransformationUniform->set(scrollModel->getTransformation());

        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        pointLightDataBuffer->unbind(GL_SHADER_STORAGE_BUFFER, 5);

        pointShadowMapTexture->unbindActive(0);

        /*
        // pointShadowMapTexture->bindActive(0);

        glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        skyboxRenderingProgram->use();
        skyboxRenderingProgram->setUniform("projection", cameraProjection);
        skyboxRenderingProgram->setUniform("view", cameraView);
        skyboxRenderingProgram->setUniform("cubeMap", 0);

        skybox->bind();
        skybox->draw();
        skybox->unbind();

        // pointShadowMapTexture->unbindActive(0);

        skyboxRenderingProgram->release();*/

        // done rendering the frame

        window.display();
    }

    return 0;
}
