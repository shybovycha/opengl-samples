#include "common/stdafx.hpp"

#include "common/AssimpModel.hpp"
#include "common/Skybox.hpp"

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

    sf::Window window(videoMode, "Hello, Reflection!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling reflection mapping vertex shader...";

    auto reflectionMappingVertexSource = globjects::Shader::sourceFromFile("media/reflection-mapping.vert");
    auto reflectionMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingVertexSource.get());
    auto reflectionMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), reflectionMappingVertexShaderTemplate.get());

    if (!reflectionMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection mapping geometry shader...";

    auto reflectionMappingGeometrySource = globjects::Shader::sourceFromFile("media/reflection-mapping.geom");
    auto reflectionMappingGeometryShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingGeometrySource.get());
    auto reflectionMappingGeometryShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_GEOMETRY_SHADER), reflectionMappingGeometryShaderTemplate.get());

    if (!reflectionMappingGeometryShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection mapping fragment shader...";

    auto reflectionMappingFragmentSource = globjects::Shader::sourceFromFile("media/reflection-mapping.frag");
    auto reflectionMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingFragmentSource.get());
    auto reflectionMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), reflectionMappingFragmentShaderTemplate.get());

    if (!reflectionMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking reflection mapping shaders..." << std::endl;

    auto reflectionMappingProgram = std::make_unique<globjects::Program>();
    reflectionMappingProgram->attach(reflectionMappingVertexShader.get(), reflectionMappingGeometryShader.get(), reflectionMappingFragmentShader.get());

    auto reflectionMappingModelTransformationUniform = reflectionMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple rendering vertex shader...";

    auto simpleRenderingVertexSource = globjects::Shader::sourceFromFile("media/simple-rendering.vert");
    auto simpleRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleRenderingVertexSource.get());
    auto simpleRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), simpleRenderingVertexShaderTemplate.get());

    if (!simpleRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple rendering mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple rendering fragment shader...";

    auto simpleRenderingFragmentSource = globjects::Shader::sourceFromFile("media/simple-rendering.frag");
    auto simpleRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleRenderingFragmentSource.get());
    auto simpleRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), simpleRenderingFragmentShaderTemplate.get());

    if (!simpleRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking simple rendering shaders..." << std::endl;

    auto simpleRenderingProgram = std::make_unique<globjects::Program>();
    simpleRenderingProgram->attach(simpleRenderingVertexShader.get(), simpleRenderingFragmentShader.get());

    auto simpleRenderingModelTransformationUniform = simpleRenderingProgram->getUniform<glm::mat4>("model");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling skybox rendering vertex shader...";

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

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection rendering vertex shader...";

    auto reflectionRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/reflection-rendering.vert");
    auto reflectionRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionRenderingVertexShaderSource.get());
    auto reflectionRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), reflectionRenderingVertexShaderTemplate.get());

    if (!reflectionRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection rendering fragment shader...";

    auto reflectionRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/reflection-rendering.frag");
    auto reflectionRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionRenderingFragmentShaderSource.get());
    auto reflectionRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), reflectionRenderingFragmentShaderTemplate.get());

    if (!reflectionRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking reflection rendering shader...";

    auto reflectionRenderingProgram = std::make_unique<globjects::Program>();
    reflectionRenderingProgram->attach(reflectionRenderingVertexShader.get(), reflectionRenderingFragmentShader.get());

    auto reflectionRenderingModelTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("model");
    auto reflectionRenderingViewTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("view");
    auto reflectionRenderingProjectionTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("projection");

    // auto reflectionRenderingLightColorUniform = reflectionRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto reflectionRenderingCameraPositionUniform = reflectionRenderingProgram->getUniform<glm::vec3>("cameraPosition");

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

    tableModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

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

    auto inkBottleScene = importer.ReadFile("media/ink-bottle.obj", 0);

    if (!inkBottleScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto inkBottleModel = AssimpModel::fromAiNode(inkBottleScene, inkBottleScene->mRootNode, { "media" });

    // TODO: encapsulate position and rotation in model class instead of just exposing transformation
    // this constant here is taken from ink bottle position
    glm::vec3 reflectiveModelPosition = glm::vec3(-1.75f, 3.85f, 1.05f);

    inkBottleModel->setTransformation(
        glm::translate(reflectiveModelPosition) *
        glm::scale(glm::vec3(0.5f))
    );

    auto penScene = importer.ReadFile("media/pen-lowpoly.obj", 0);

    if (!penScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto penModel = AssimpModel::fromAiNode(penScene, penScene->mRootNode, { "media" });

    // rotate -> scale -> translate; can be done as series of matrix multiplications M_translation * M_scale * M_rotation
    // each of the components, in turn, can also be a series of matrix multiplications: M_rotation = M_rotate_z * M_rotate_y * M_rotate_x
    penModel->setTransformation(
        glm::translate(glm::vec3(0.35f, 3.95f, -0.75f)) *
        glm::scale(glm::vec3(0.05f)) *
        (glm::rotate(glm::radians(12.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
    );

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing reflectionMapTexture...";

    auto reflectionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    reflectionMapTexture->bind();

    const auto reflectionMapSize = 2048;

    for (auto i = 0; i < 6; ++i)
    {
        ::glTexImage2D(
            static_cast<::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
            0,
            GL_RGBA8,
            reflectionMapSize,
            reflectionMapSize,
            0,
            GL_RGBA,
            GL_UNSIGNED_INT,
            nullptr);
    }

    reflectionMapTexture->unbind();

    auto reflectionMapDepthTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    reflectionMapDepthTexture->bind();

    for (auto i = 0; i < 6; ++i)
    {
        ::glTexImage2D(
            static_cast<::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
            0,
            GL_DEPTH_COMPONENT,
            reflectionMapSize,
            reflectionMapSize,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
    }

    reflectionMapDepthTexture->unbind();

    /*auto skybox = Skybox::fromCubemap(reflectionMapTexture.get())
        ->size(40.0f)
        ->build();*/

    auto skybox = Skybox::builder()
        ->top("media/skybox-top.png")
        ->bottom("media/skybox-bottom.png")
        ->left("media/skybox-left.png")
        ->right("media/skybox-right.png")
        ->front("media/skybox-front.png")
        ->back("media/skybox-back.png")
        ->size(40.0f)
        ->build();

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing reflection mapping frame buffer...";

    auto reflectionMappingFramebuffer = std::make_unique<globjects::Framebuffer>();
    reflectionMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), reflectionMapTexture.get());
    reflectionMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), reflectionMapDepthTexture.get());

    // tell framebuffer it actually needs to render to **BOTH** textures, but does not have to output anywhere (last NONE argument, iirc)
    reflectionMappingFramebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });

    reflectionMappingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

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

        // this is a cubemap, hence aspect ratio **must** be 1:1
        glm::mat4 reflectionProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

        // TODO: technically, this should be calculated as AABB / 2
        const auto reflectionOffset = glm::vec3(0.0f, 0.05f, 0.0f);

        reflectionMappingProgram->setUniform("projectionViewMatrices[0]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[1]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[2]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[3]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[4]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[5]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // first render pass - shadow mapping
        reflectionMappingFramebuffer->bind();

        ::glViewport(0, 0, reflectionMapSize, reflectionMapSize);
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_BACK);

        reflectionMappingProgram->use();

        reflectionMappingProgram->setUniform("diffuseTexture", 1);

        reflectionMappingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        reflectionMappingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        reflectionMappingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        reflectionMappingModelTransformationUniform->set(scrollModel->getTransformation());

        // scroll model needs culling to be disabled since this is a modified plane, so...
        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        reflectionMappingModelTransformationUniform->set(penModel->getTransformation());

        penModel->bind();
        penModel->draw();
        penModel->unbind();

        reflectionMappingFramebuffer->unbind();

        reflectionMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        skyboxRenderingProgram->use();
        skyboxRenderingProgram->setUniform("projection", cameraProjection);
        skyboxRenderingProgram->setUniform("view", glm::mat4(glm::mat3(cameraView)));

#ifndef _DEBUG
        reflectionMapTexture->bindActive(0);
        skyboxRenderingProgram->setUniform("cubeMap", 0);
#else
        skyboxRenderingProgram->setUniform("cubeMap", 1);
#endif

        skybox->bind();
        skybox->draw();
        skybox->unbind();

        reflectionMapTexture->unbindActive(0);

        skyboxRenderingProgram->release();

        // render object with reflective material

        reflectionRenderingProgram->use();

        reflectionRenderingProgram->setUniform("projection", cameraProjection);
        reflectionRenderingProgram->setUniform("view", cameraView);
        reflectionRenderingProgram->setUniform("model", inkBottleModel->getTransformation());

        reflectionRenderingProgram->setUniform("cameraPosition", cameraPos);

        reflectionMapTexture->bindActive(0);

        reflectionRenderingProgram->setUniform("reflectionMap", 0);
        reflectionRenderingProgram->setUniform("diffuseTexture", 1);

        inkBottleModel->bind();
        inkBottleModel->draw();
        inkBottleModel->unbind();

        reflectionMapTexture->unbindActive(0);

        reflectionRenderingProgram->release();

        // draw the scene

        simpleRenderingProgram->use();

        simpleRenderingProgram->setUniform("projection", cameraProjection);
        simpleRenderingProgram->setUniform("view", cameraView);

        simpleRenderingProgram->setUniform("diffuseTexture", 1);

        simpleRenderingModelTransformationUniform->set(houseModel->getTransformation());

#ifdef _DEBUG
        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();
#endif

        simpleRenderingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        simpleRenderingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        simpleRenderingModelTransformationUniform->set(penModel->getTransformation());

        penModel->bind();
        penModel->draw();
        penModel->unbind();

        simpleRenderingModelTransformationUniform->set(scrollModel->getTransformation());

        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        simpleRenderingProgram->release();

        // done rendering the frame

        window.display();
    }

    return 0;
}
