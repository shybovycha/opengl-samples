#include "common/stdafx.hpp"

#include "common/AssimpModel.hpp"
#include "common/SingleMeshModel.hpp"

class Skybox;

class SkyboxBuilder
{
    friend class Skybox;

public:
    SkyboxBuilder* size(float size)
    {
        m_size = size;
        return this;
    }

    SkyboxBuilder* top(std::string filename)
    {
        m_top.loadFromFile(filename);
        return this;
    }

    SkyboxBuilder* bottom(std::string filename)
    {
        m_bottom.loadFromFile(filename);
        return this;
    }

    SkyboxBuilder* left(std::string filename)
    {
        m_left.loadFromFile(filename);
        return this;
    }

    SkyboxBuilder* right(std::string filename)
    {
        m_right.loadFromFile(filename);
        return this;
    }

    SkyboxBuilder* front(std::string filename)
    {
        m_front.loadFromFile(filename);
        return this;
    }

    SkyboxBuilder* back(std::string filename)
    {
        m_back.loadFromFile(filename);
        return this;
    }

    std::unique_ptr<Skybox> build()
    {
        std::map<gl::GLenum, sf::Image> skyboxTextures{
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X), m_right },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X), m_left },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y), m_top },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y), m_bottom },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z), m_back },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z), m_front },
        };

        auto skyboxTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<GLint>(GL_CLAMP_TO_EDGE));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<GLint>(GL_CLAMP_TO_EDGE));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<GLint>(GL_CLAMP_TO_EDGE));

        skyboxTexture->bind();

        for (auto& kv : skyboxTextures)
        {
            const auto target = kv.first;
            auto& image = kv.second;

            if (target == gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y || target == gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
            {
                image.flipVertically();
            }
            else
            {
                image.flipHorizontally();
            }

            ::glTexImage2D(
                static_cast<::GLenum>(target),
                0,
                static_cast<::GLenum>(GL_RGBA8),
                static_cast<::GLsizei>(image.getSize().x),
                static_cast<::GLsizei>(image.getSize().y),
                0,
                static_cast<::GLenum>(GL_RGBA),
                static_cast<::GLenum>(GL_UNSIGNED_BYTE),
                reinterpret_cast<const ::GLvoid*>(image.getPixelsPtr()));
        }

        skyboxTexture->unbind();

        std::vector<glm::vec3> vertices{
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, -1.0f },
        };

        std::vector<glm::vec3> normals{
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },

            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },

            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },

            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },

            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },

            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },

            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },

            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
        };

        std::for_each(vertices.begin(), vertices.end(), [this](glm::vec3 p) { return p * m_size; });

        std::vector<unsigned int> indices{
            2, 1, 0,
            2, 0, 3,
            4, 5, 6,
            7, 4, 6,
            10, 9, 8,
            10, 8, 11,
            12, 13, 14,
            15, 12, 14,
            18, 17, 16,
            18, 16, 19,
            20, 21, 22,
            23, 20, 22,
        };

        std::vector<glm::vec2> uvs{
            { 0.333333f, 0.500000f },
            { 0.333333f, 0.000000f },
            { 0.000000f, 0.000000f },
            { 0.000000f, 0.500000f },
            { 0.000000f, 1.000000f },
            { 0.000000f, 0.500000f },
            { 0.333333f, 0.500000f },
            { 0.333333f, 1.000000f },
            { 1.000000f, 1.000000f },
            { 1.000000f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 1.000000f },
            { 0.333333f, 1.000000f },
            { 0.333333f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 1.000000f },
            { 0.340000f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 0.000000f },
            { 0.340000f, 0.000000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 0.000000f },
            { 1.000000f, 0.000000f },
            { 1.000000f, 0.500000f },
        };

        auto mesh = AbstractMesh::builder()
            ->addVertices(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->addTexture(std::move(skyboxTexture))
            ->build();

        return std::make_unique<Skybox>(std::move(mesh));
    }

private:
    SkyboxBuilder() : m_size(1.0f) {}

    sf::Image m_top, m_bottom, m_left, m_right, m_front, m_back;
    float m_size;
};

class Skybox : public SingleMeshModel
{
    friend class SkyboxBuilder;

public:
    static SkyboxBuilder* builder()
    {
        return new SkyboxBuilder();
    }

    Skybox(std::unique_ptr<AbstractMesh> mesh) : SingleMeshModel(std::move(mesh))
    {
    }
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

    sf::Window window(videoMode, "Hello, Skybox!", sf::Style::Default, settings);

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
        std::cerr << "[ERROR] Can not compile shadow rendering fragment shader" << std::endl;
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

    std::cout << "[INFO] Compiling skybox rendering vertex shader...";

    auto skyboxRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/skybox.vert");
    auto skyboxRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingVertexShaderSource.get());
    auto skyboxRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), skyboxRenderingVertexShaderTemplate.get());

    if (!skyboxRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile skybox rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling skybox rendering fragment shader...";

    auto skyboxRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/skybox.frag");
    auto skyboxRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingFragmentShaderSource.get());
    auto skyboxRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), skyboxRenderingFragmentShaderTemplate.get());

    if (!skyboxRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile skybox rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking skybox rendering shader...";

    auto skyboxRenderingProgram = std::make_unique<globjects::Program>();
    skyboxRenderingProgram->attach(skyboxRenderingVertexShader.get(), skyboxRenderingFragmentShader.get());

    auto skyboxRenderingViewTransformationUniform = skyboxRenderingProgram->getUniform<glm::mat4>("view");
    auto skyboxRenderingProjectionTransformationUniform = skyboxRenderingProgram->getUniform<glm::mat4>("projection");

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

    auto skybox = Skybox::builder()
        ->size(10.0f)
        ->right("media/skybox-right.png")
        ->left("media/skybox-left.png")
        ->top("media/skybox-top.png")
        ->bottom("media/skybox-bottom.png")
        ->back("media/skybox-back.png")
        ->front("media/skybox-front.png")
        ->build();

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

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
    glDepthFunc(GL_LEQUAL);

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

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
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

        glDepthMask(GL_FALSE);

        skyboxRenderingProgram->use();

        skyboxRenderingProjectionTransformationUniform->set(cameraProjection);
        skyboxRenderingViewTransformationUniform->set(glm::mat4(glm::mat3(cameraView)));

        skyboxRenderingProgram->setUniform("cubeMap", 1);

        skybox->bind();
        skybox->draw();
        skybox->unbind();

        skyboxRenderingProgram->release();

        glDepthMask(GL_TRUE);

        // done rendering the frame

        window.display();
    }

    return 0;
}
