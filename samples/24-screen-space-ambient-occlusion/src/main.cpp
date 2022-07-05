#include "common/stdafx.hpp"

#include "common/AssimpModel.hpp"
#include "common/Skybox.hpp"

struct alignas(16) PointLightDescriptor
{
    glm::vec3 position;
    float strength;
    glm::vec4 color;
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

    sf::Window window(videoMode, "Hello, SSAO!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling deferred rendering pre-pass vertex shader...";

    auto deferredRenderingPrePassVertexSource = globjects::Shader::sourceFromFile("media/deferred-rendering-pre-pass.vert");
    auto deferredRenderingPrePassVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingPrePassVertexSource.get());
    auto deferredRenderingPrePassVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), deferredRenderingPrePassVertexShaderTemplate.get());

    if (!deferredRenderingPrePassVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering pre-pass vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering pre-pass fragment shader...";

    auto deferredRenderingPrePassFragmentSource = globjects::Shader::sourceFromFile("media/deferred-rendering-pre-pass.frag");
    auto deferredRenderingPrePassFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingPrePassFragmentSource.get());
    auto deferredRenderingPrePassFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), deferredRenderingPrePassFragmentShaderTemplate.get());

    if (!deferredRenderingPrePassFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering pre-pass fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking deferred rendering pre-pass shaders..." << std::endl;

    auto deferredRenderingPrePassProgram = std::make_unique<globjects::Program>();
    deferredRenderingPrePassProgram->attach(deferredRenderingPrePassVertexShader.get(), deferredRenderingPrePassFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering final pass vertex shader...";

    auto deferredRenderingFinalPassVertexSource = globjects::Shader::sourceFromFile("media/deferred-rendering-final-pass.vert");
    auto deferredRenderingFinalPassVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingFinalPassVertexSource.get());
    auto deferredRenderingFinalPassVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), deferredRenderingFinalPassVertexShaderTemplate.get());

    if (!deferredRenderingFinalPassVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering final pass vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering final pass fragment shader...";

    auto deferredRenderingFinalPassFragmentSource = globjects::Shader::sourceFromFile("media/deferred-rendering-final-pass.frag");
    auto deferredRenderingFinalPassFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingFinalPassFragmentSource.get());
    auto deferredRenderingFinalPassFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), deferredRenderingFinalPassFragmentShaderTemplate.get());

    if (!deferredRenderingFinalPassFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering final pass fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking deferred rendering final pass shaders..." << std::endl;

    auto deferredRenderingFinalPassProgram = std::make_unique<globjects::Program>();
    deferredRenderingFinalPassProgram->attach(deferredRenderingFinalPassVertexShader.get(), deferredRenderingFinalPassFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling SSAO vertex shader...";

    auto ssaoVertexSource = globjects::Shader::sourceFromFile("media/ssao.vert");
    auto ssaoVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(ssaoVertexSource.get());
    auto ssaoVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), ssaoVertexShaderTemplate.get());

    if (!ssaoVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile SSAO vertex shader" << std::endl;
        return 1;
    }

    std::cout << "[INFO] Compiling SSAO fragment shader...";

    auto ssaoFragmentSource = globjects::Shader::sourceFromFile("media/ssao.frag");
    auto ssaoFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(ssaoFragmentSource.get());
    auto ssaoFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), ssaoFragmentShaderTemplate.get());

    if (!ssaoFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile SSAO fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking SSAO shaders..." << std::endl;

    auto ssaoProgram = std::make_unique<globjects::Program>();
    ssaoProgram->attach(ssaoVertexShader.get(), ssaoFragmentShader.get());

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

    std::cout << "[INFO] Compiling SSAO vertex shader...";

    auto blurVertexSource = globjects::Shader::sourceFromFile("media/blur.vert");
    auto blurVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(blurVertexSource.get());
    auto blurVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), blurVertexShaderTemplate.get());

    if (!blurVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile blur vertex shader" << std::endl;
        return 1;
    }

    std::cout << "[INFO] Compiling blur fragment shader...";

    auto blurFragmentSource = globjects::Shader::sourceFromFile("media/blur.frag");
    auto blurFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(blurFragmentSource.get());
    auto blurFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), blurFragmentShaderTemplate.get());

    if (!blurFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile blur fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking blur shaders..." << std::endl;

    auto blurProgram = std::make_unique<globjects::Program>();
    blurProgram->attach(blurVertexShader.get(), blurFragmentShader.get());

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

    std::cout << "[INFO] Compiling simple vertex shader...";

    auto simpleVertexSource = globjects::Shader::sourceFromFile("media/simple-rendering.vert");
    auto simpleVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleVertexSource.get());
    auto simpleVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), simpleVertexShaderTemplate.get());

    if (!simpleVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple fragment shader...";

    auto simpleFragmentSource = globjects::Shader::sourceFromFile("media/simple-rendering.frag");
    auto simpleFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleFragmentSource.get());
    auto simpleFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), simpleFragmentShaderTemplate.get());

    if (!simpleFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking simple shaders..." << std::endl;

    auto simpleProgram = std::make_unique<globjects::Program>();
    simpleProgram->attach(simpleVertexShader.get(), simpleFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto quadScene = importer.ReadFile("media/quad.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!quadScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto quadModel = AssimpModel::fromAiNode(quadScene, quadScene->mRootNode);

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

    sf::Image penNormalMapImage;

    if (!penNormalMapImage.loadFromFile("media/pen-normal.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    penNormalMapImage.flipVertically();

    auto penNormalMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    penNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    penNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    penNormalMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(penNormalMapImage.getSize().x, penNormalMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(penNormalMapImage.getPixelsPtr()));

    sf::Image inkBottleNormalMapImage;

    if (!inkBottleNormalMapImage.loadFromFile("media/ink-bottle-normal.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    inkBottleNormalMapImage.flipVertically();

    auto inkBottleNormalMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    inkBottleNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    inkBottleNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    inkBottleNormalMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(inkBottleNormalMapImage.getSize().x, inkBottleNormalMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(inkBottleNormalMapImage.getPixelsPtr()));

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

    inkBottleModel->setTransformation(
        glm::translate(glm::vec3(-1.75f, 3.85f, 1.05f)) *
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

    std::cout << "[DEBUG] Initializing skybox...";

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

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing temporary frame buffers...";

    auto temporaryTexture1 = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    temporaryTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    temporaryTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    temporaryTexture1->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        nullptr
    );

    auto temporaryFramebuffer = std::make_unique<globjects::Framebuffer>();

    temporaryFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), temporaryTexture1.get());
    temporaryFramebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });

    temporaryFramebuffer->printStatus(true);

    auto temporaryTexture2 = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    temporaryTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    temporaryTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    temporaryTexture2->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        nullptr
    );

    auto temporaryFramebuffer2 = std::make_unique<globjects::Framebuffer>();

    temporaryFramebuffer2->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), temporaryTexture2.get());
    temporaryFramebuffer2->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });

    temporaryFramebuffer2->printStatus(true);

    std::cout << "[DEBUG] done" << std::endl;

    std::cout << "[DEBUG] Initializing deferred rendering frame buffer...";

    auto deferredFragmentPositionTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentPositionTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentPositionTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentPositionTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB32F),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredFragmentNormalTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentNormalTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentNormalTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentNormalTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB32F),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredFragmentAlbedoTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentAlbedoTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentAlbedoTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentAlbedoTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        nullptr
    );

    auto deferredFragmentDepthTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentDepthTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredRenderingFramebuffer = std::make_unique<globjects::Framebuffer>();
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), deferredFragmentPositionTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1), deferredFragmentNormalTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT2), deferredFragmentAlbedoTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), deferredFragmentDepthTexture.get());

    // tell framebuffer it actually needs to render to **BOTH** textures, but does not have to output anywhere (last NONE argument, iirc)
    deferredRenderingFramebuffer->setDrawBuffers({
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1),
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT2),
        static_cast<gl::GLenum>(GL_NONE)
    });

    deferredRenderingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Preparing SSAO kernels...";

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;

    const auto ssaoKernelSamples = 64;

    std::vector<glm::vec3> ssaoKernel;

    for (unsigned int i = 0; i < ssaoKernelSamples; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
        );

        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = static_cast<float>(i) / static_cast<float>(ssaoKernelSamples);

        // lerp(a, b, f) = a + f * (b - a);
        // scale = lerp(0.1f, 1.0f, scale * scale);
        scale = 0.1f + (scale * scale * (1.0f - 0.1f));

        sample *= scale;

        ssaoKernel.push_back(sample);
    }

    std::vector<glm::vec3> ssaoNoise;

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            0.0f
        );

        ssaoNoise.push_back(noise);
    }

    auto ssaoNoiseTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    ssaoNoiseTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_NEAREST));
    ssaoNoiseTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_NEAREST));
    ssaoNoiseTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<GLint>(GL_REPEAT));
    ssaoNoiseTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<GLint>(GL_REPEAT));

    ssaoNoiseTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB32F),
        glm::vec2(4.0f, 4.0f),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        &ssaoNoise[0]
    );

    auto ssaoKernelTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_1D));

    ssaoKernelTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_NEAREST));
    ssaoKernelTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_NEAREST));

    ssaoKernelTexture->image1D(
        0,
        static_cast<gl::GLenum>(GL_RGBA16F),
        static_cast<float>(ssaoKernelSamples),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        &ssaoKernel[0]
    );

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Preparing data buffers...";

    std::vector<PointLightDescriptor> pointLights{ { glm::vec3(-1.75f, 3.85f, -0.75f), 0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) } };

    auto pointLightDataBuffer = std::make_unique<globjects::Buffer>();

    pointLightDataBuffer->setData(pointLights, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

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

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y), 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        const float nearPlane = 0.1f;
        const float farPlane = 10.0f;

        // first render pass - prepare for deferred rendering by rendering to the entire scene to a deferred rendering framebuffer's attachments
        {
            deferredRenderingFramebuffer->bind();

            ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
            ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);

            /*skyboxRenderingProgram->use();
            skyboxRenderingProgram->setUniform("projection", cameraProjection);
            skyboxRenderingProgram->setUniform("view", glm::mat4(glm::mat3(cameraView)));

            skyboxRenderingProgram->setUniform("cubeMap", 1);

            skybox->bind();
            skybox->draw();
            skybox->unbind();

            skyboxRenderingProgram->release();*/

            deferredRenderingPrePassProgram->use();

            deferredRenderingPrePassProgram->setUniform("projection", cameraProjection);
            deferredRenderingPrePassProgram->setUniform("view", cameraView);

            deferredRenderingPrePassProgram->setUniform("diffuseTexture", 1);
            deferredRenderingPrePassProgram->setUniform("normalMapTexture", 2);

            deferredRenderingPrePassProgram->setUniform("model", houseModel->getTransformation());

            houseModel->bind();
            houseModel->draw();
            houseModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", tableModel->getTransformation());

            tableModel->bind();
            tableModel->draw();
            tableModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", lanternModel->getTransformation());

            lanternModel->bind();
            lanternModel->draw();
            lanternModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", penModel->getTransformation());

            penNormalMapTexture->bindActive(2);

            penModel->bind();
            penModel->draw();
            penModel->unbind();

            penNormalMapTexture->unbindActive(2);

            deferredRenderingPrePassProgram->setUniform("model", inkBottleModel->getTransformation());

            inkBottleNormalMapTexture->bindActive(2);

            inkBottleModel->bind();
            inkBottleModel->draw();
            inkBottleModel->unbind();

            inkBottleNormalMapTexture->unbindActive(2);

            deferredRenderingPrePassProgram->setUniform("model", scrollModel->getTransformation());

            glDisable(GL_CULL_FACE);

            scrollModel->bind();
            scrollModel->draw();
            scrollModel->unbind();

            glEnable(GL_CULL_FACE);

            deferredRenderingPrePassProgram->release();

            deferredRenderingFramebuffer->unbind();
        }

        // second render pass - calculate & blur the ambient occlusion
        {
            temporaryFramebuffer->bind();

            ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
            ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ssaoProgram->use();

            deferredFragmentPositionTexture->bindActive(2);
            deferredFragmentNormalTexture->bindActive(3);

            ssaoNoiseTexture->bindActive(5);
            ssaoKernelTexture->bindActive(6);

            ssaoProgram->setUniform("positionTexture", 2);
            ssaoProgram->setUniform("normalTexture", 3);
            ssaoProgram->setUniform("albedoTexture", 4);

            ssaoProgram->setUniform("ssaoNoiseTexture", 5);
            ssaoProgram->setUniform("ssaoKernelTexture", 6);

            ssaoProgram->setUniform("cameraPosition", cameraPos);
            ssaoProgram->setUniform("projection", cameraProjection);
            ssaoProgram->setUniform("view", cameraView);

            quadModel->bind();
            quadModel->draw();
            quadModel->unbind();

            deferredFragmentPositionTexture->unbindActive(2);
            deferredFragmentNormalTexture->unbindActive(3);

            ssaoNoiseTexture->unbindActive(5);
            ssaoKernelTexture->unbindActive(6);

            ssaoProgram->release();

            temporaryFramebuffer->unbind();

            // blur
            temporaryFramebuffer->bind(static_cast<gl::GLenum>(GL_READ_FRAMEBUFFER));
            temporaryFramebuffer2->bind(static_cast<gl::GLenum>(GL_DRAW_FRAMEBUFFER));

            temporaryFramebuffer->blit(
                static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
                std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
                temporaryFramebuffer2.get(),
                std::vector<gl::GLenum>{ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0) },
                std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
                static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT),
                static_cast<gl::GLenum>(GL_NEAREST));

            // same as
            // glReadBuffer(GL_COLOR_ATTACHMENT0);
            // glDrawBuffer(GL_COLOR_ATTACHMENT0);
            // glBlitFramebuffer(0, 0, window.getSize().x, window.getSize().y, 0, 0, window.getSize().x, window.getSize().y, static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT), static_cast<gl::GLenum>(GL_NEAREST));

            temporaryFramebuffer->unbind();
            temporaryFramebuffer2->unbind();

            blurProgram->use();

            const auto blurPasses = 10;

            // for the initial blur pass, use the texture from the bloomFramebuffer as an input

            // we do not need anything extra here, since the bloomBlurFramebuffer2 (which we read from) will already contain the data from the bloomBrightnessTexture

            blurProgram->setUniform("blurInput", 0);

            for (auto i = 0; i < blurPasses; ++i)
            {
                // bind one framebuffer to write blur results to and bind the texture from another framebuffer to read input data from (for this blur stage)
                if (i % 2 == 0)
                {
                    // bind the new target framebuffer to write blur results to
                    temporaryFramebuffer->bind();
                    // bind the texture from the previous blur pass to read input data for this stage from
                    temporaryTexture2->bindActive(0);
                    // tell shader that we want to use horizontal blur
                    blurProgram->setUniform("isHorizontalBlur", true);
                }
                else
                {
                    // bind the new target framebuffer to write blur results to
                    temporaryFramebuffer2->bind();
                    // bind the texture from the previous blur pass to read input data for this stage from
                    if (i > 0)
                        temporaryTexture1->bindActive(0);
                    // tell shader that we want to use vertical blur
                    blurProgram->setUniform("isHorizontalBlur", false);
                }

                // render quad with the texture from the active texture
                quadModel->bind();
                quadModel->draw();
                quadModel->unbind();

                if (i % 2 == 0)
                {
                    // unbind the active framebuffer
                    temporaryFramebuffer->unbind();
                    // unbind the active texture
                    temporaryTexture2->unbindActive(0);
                }
                else
                {
                    temporaryFramebuffer2->unbind();
                    temporaryTexture1->unbindActive(0);
                }
            }

            blurProgram->release();
        }

        // third render pass - merge textures from the deferred rendering pre-pass into a final frame
        {
            ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
            ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            deferredRenderingFinalPassProgram->use();

            deferredFragmentPositionTexture->bindActive(2);
            deferredFragmentNormalTexture->bindActive(3);
            deferredFragmentAlbedoTexture->bindActive(4);

            temporaryTexture1->bindActive(5);

            pointLightDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 5);

            deferredRenderingFinalPassProgram->setUniform("positionTexture", 2);
            deferredRenderingFinalPassProgram->setUniform("normalTexture", 3);
            deferredRenderingFinalPassProgram->setUniform("albedoTexture", 4);

            deferredRenderingFinalPassProgram->setUniform("ssaoTexture", 5);

            deferredRenderingFinalPassProgram->setUniform("cameraPosition", cameraPos);
            deferredRenderingFinalPassProgram->setUniform("projection", cameraProjection);
            deferredRenderingFinalPassProgram->setUniform("view", cameraView);

            quadModel->bind();
            quadModel->draw();
            quadModel->unbind();

            pointLightDataBuffer->unbind(GL_SHADER_STORAGE_BUFFER, 5);

            deferredFragmentPositionTexture->unbindActive(2);
            deferredFragmentNormalTexture->unbindActive(3);
            deferredFragmentAlbedoTexture->unbindActive(4);

            temporaryTexture1->unbindActive(5);

            deferredRenderingFinalPassProgram->release();
        }

        // done rendering the frame

        window.display();
    }

    return 0;
}
