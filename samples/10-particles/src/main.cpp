#include "common/stdafx.hpp"

#include "common/Model.hpp"

class Particle {
public:
    Particle() :
        m_position(glm::vec3(0.0f)),
        m_velocity(glm::vec3(0.0f)),
        m_scale(1.0f),
        m_lifetime(1.0f)
    {
    }

    float getLifetime() const
    {
        return m_lifetime;
    }

    float getScale() const
    {
        return m_scale;
    }

    glm::vec3 getPosition() const
    {
        return m_position;
    }

    glm::vec3 getVelocity() const
    {
        return m_velocity;
    }

    bool isAlive() const
    {
        return m_lifetime > 0;
    }

    virtual glm::mat4 getModelMatrix() const
    {
        return glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(m_scale)), m_position);
    }

    void setLifetime(float lifetime)
    {
        m_lifetime = lifetime;
    }

    void setScale(float scale)
    {
        m_scale = scale;
    }

    void setPosition(glm::vec3 position)
    {
        m_position = position;
    }

    void setVelocity(glm::vec3 velocity)
    {
        m_velocity = velocity;
    }

    void kill()
    {
        m_lifetime = 0.0f;
    }

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_scale;
    float m_lifetime;
};

template <class TParticle>
class AbstractParticleAttributeGenerator
{
public:
    virtual void generate(TParticle* particle) = 0;
};

template <class TParticle>
class AbstractParticleEmitter
{
public:
    virtual void emit(TParticle* particle) = 0;
};

template <class TParticle>
class AbstractParticleAffector
{
public:
    virtual void affect(TParticle* particle, float deltaTime) = 0;
};

template <class TParticle>
class AbstractParticleRenderer
{
public:
    virtual void draw(TParticle* particle, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) = 0;
};

template <class TParticle>
class ParticleSystem
{
public:
    ParticleSystem(
        unsigned int amount,
        std::unique_ptr<AbstractParticleEmitter<TParticle>> emitter,
        std::vector<std::shared_ptr<AbstractParticleAffector<TParticle>>> affectors,
        std::unique_ptr<AbstractParticleRenderer<TParticle>> renderer
    ) :
        m_amount(amount),
        m_emitter(std::move(emitter)),
        m_renderer(std::move(renderer)),
        m_affectors(affectors)
    {
        m_particles.reserve(m_amount);

        for (auto i = 0; i < m_amount; ++i)
        {
            m_particles.push_back(std::make_unique<TParticle>());
        }
    }

    void update(float deltaTime)
    {
        for (auto& particle : m_particles)
        {
            if (!particle->isAlive())
            {
                m_emitter->emit(particle.get());
                continue;
            }

            for (auto& affector : m_affectors)
            {
                affector->affect(particle.get(), deltaTime);
            }
        }
    }

    void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
    {
        for (auto& particle : m_particles)
        {
            m_renderer->draw(particle.get(), projectionMatrix, viewMatrix);
        }
    }

private:
    std::vector<std::unique_ptr<TParticle>> m_particles;
    unsigned int m_amount;

    std::unique_ptr<AbstractParticleEmitter<TParticle>> m_emitter;
    std::unique_ptr<AbstractParticleRenderer<TParticle>> m_renderer;
    std::vector<std::shared_ptr<AbstractParticleAffector<TParticle>>> m_affectors;
};

class SimpleParticle : public Particle
{
public:
    SimpleParticle() : Particle(), m_mass(0.0f), m_rotation(0.0f) {}

    float getMass() const
    {
        return m_mass;
    }

    float getRotation() const
    {
        return m_rotation;
    }

    glm::mat4 getModelMatrix() const override
    {
        return glm::translate(
            glm::rotate(
                glm::scale(glm::mat4(1.0f), glm::vec3(getScale())),
                glm::radians(m_rotation),
                glm::vec3(0.0f, 0.0f, 1.0f)
            ),
            getPosition()
        );
    }

    void setMass(float mass)
    {
        m_mass = mass;
    }

    void setRotation(float rotation)
    {
        m_rotation = rotation;
    }

private:
    float m_mass;
    float m_rotation;
};

class SimpleParticleEmitter : public AbstractParticleEmitter<SimpleParticle>
{
public:
    SimpleParticleEmitter(
        float lifetime,
        glm::vec3 position,
        glm::vec3 velocity,
        float scale,
        float mass
    ) :
        m_lifetime(lifetime),
        m_origin(position),
        m_velocity(velocity),
        m_scale(scale),
        m_mass(mass)
    {}

    void emit(SimpleParticle* particle) override
    {
        particle->setLifetime(m_lifetime * static_cast<float>((std::rand() % 473) / 473.0f));
        particle->setPosition(m_origin);
        particle->setVelocity(glm::normalize(m_velocity) * static_cast<float>((std::rand() % 439) / 439.0f));
        particle->setMass(m_mass * static_cast<float>((std::rand() % 173) / 173.0f));
        particle->setRotation(glm::radians(glm::pi<float>() * 0.5f));
        particle->setScale(m_scale * static_cast<float>((std::rand() % 93) / 93.0f));
    }

private:
    glm::vec3 m_velocity;
    glm::vec3 m_origin;
    float m_lifetime;
    float m_mass;
    float m_scale;
};

class SimpleParticleAffector : public AbstractParticleAffector<SimpleParticle>
{
public:
    const glm::vec3 GRAVITY{ 0.0f, -9.8f, 0.0f };

    void affect(SimpleParticle* particle, float deltaTime) override
    {
        float speed = bezier<3>(particle->getLifetime(), 0.32f, 0.0f, 1.0f, 0.12f);

        particle->setLifetime(particle->getLifetime() - deltaTime);
        particle->setVelocity(particle->getVelocity() + speed * particle->getMass() * GRAVITY * deltaTime);
        particle->setPosition(particle->getPosition() + particle->getVelocity() * deltaTime);
        particle->setRotation(particle->getRotation() + 2.0f * deltaTime);
    }

protected:
    template <unsigned int splineOrder, unsigned int iteration>
    float bezier(float t, float p_0)
    {
        return p_0 * std::powf(1.0f - t, static_cast<float>(splineOrder));
    }

    /*! \brief Calculate interpolated value using the Bezier curve
    * \param splineOrder the order of the Bezier curve; 2 for quadratic, 3 for cubic, etc.
    * \param t the parameter for the Bezier function
    * \param args the list of points defining the Bezier curve; must be \p splineOrder + 1
    * \return The function \f$y = B_{splineOrder}(x)\f$ applied to argument \p t
    */
    template <unsigned int splineOrder, unsigned int iteration = splineOrder, typename... TArgs>
    float bezier(float t, float p_i, TArgs... args)
    {
        return bezier<splineOrder, iteration - 1>(t, args...);
    }

    //! Generates polynomial coefficients
    /*!
    * \param n total number of polynomial coefficients
    * \param i current polynomial coefficient index
    */
    float polynomial(unsigned int n, unsigned int i)
    {
        static std::map<std::pair<unsigned int, unsigned int>, float> cache;

        const auto key = std::make_pair(n, i);

        if (cache.contains(key))
        {
            return cache[key];
        }

        const auto value = factorial(n) / (factorial(i) * factorial(n - i));

        cache[key] = value;

        return value;
    }

    unsigned int factorial(unsigned int n)
    {
        static std::map<unsigned int, unsigned int> cache;

        if (cache.contains(n))
        {
            return cache[n];
        }

        if (n == 0)
        {
            return 1;
        }

        const auto value = n * factorial(n - 1);

        cache[n] = value;

        return value;
    }
};

class SimpleParticleRenderer : public AbstractParticleRenderer<SimpleParticle>
{
public:
    SimpleParticleRenderer(std::unique_ptr<Model> model, std::unique_ptr<globjects::Texture> texture) : m_model(std::move(model)), m_texture(std::move(texture))
    {
        std::cout << "[INFO] Compiling particle rendering vertex shader...";

        auto particleRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/particle.vert");
        auto particleRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(particleRenderingVertexShaderSource.get());
        m_particleRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), particleRenderingVertexShaderTemplate.get());

        if (!m_particleRenderingVertexShader->compile())
        {
            std::cerr << "[ERROR] Can not compile particle rendering vertex shader" << std::endl;
            // TODO: throw an exception?
        }

        std::cout << "done" << std::endl;

        std::cout << "[INFO] Compiling particle rendering fragment shader...";

        auto particleRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/particle.frag");
        auto particleRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(particleRenderingFragmentShaderSource.get());
        m_particleRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), particleRenderingFragmentShaderTemplate.get());

        if (!m_particleRenderingFragmentShader->compile())
        {
            std::cerr << "[ERROR] Can not compile particle fragment shader" << std::endl;
            // TODO: throw an exception?
        }

        std::cout << "done" << std::endl;

        std::cout << "[INFO] Linking particle rendering shaders...";

        m_particleRenderingProgram = std::make_unique<globjects::Program>();
        m_particleRenderingProgram->attach(m_particleRenderingVertexShader.get(), m_particleRenderingFragmentShader.get());

        m_transformationMatrixUniform = m_particleRenderingProgram->getUniform<glm::mat4>("transformationMatrix");
        m_lifetimeUniform = m_particleRenderingProgram->getUniform<float>("lifetime");

        std::cout << "done" << std::endl;
    }

    void draw(SimpleParticle* particle, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) override
    {
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glDepthMask(false);

        m_particleRenderingProgram->use();

        glm::mat4 modelMatrix = particle->getModelMatrix();

        /*
        * reset the rotation for the particles by replacing the model matrix' top 3x3 sub-matrix, containing the rotation and scale,
        * with the transposed top 3x3 sub-matrix of the view matrix, as per ThinMatrix' particles tutorial.
        *
        * this effectively makes the result of multiplication viewMatrix * modelMatrix have an identity matrix at the top 3x3 sub-matrix.
        *
        * hence after the multiplication we have to scale and rotate the model matrix again, this time in the "camera space", so to speak
        * meaning the particle is already facing camera, so we can scale and rotate it relatively to itself
        */
        modelMatrix[0][0] = viewMatrix[0][0];
        modelMatrix[0][1] = viewMatrix[1][0];
        modelMatrix[0][2] = viewMatrix[2][0];

        modelMatrix[1][0] = viewMatrix[0][1];
        modelMatrix[1][1] = viewMatrix[1][1];
        modelMatrix[1][2] = viewMatrix[2][1];

        modelMatrix[2][0] = viewMatrix[0][2];
        modelMatrix[2][1] = viewMatrix[1][2];
        modelMatrix[2][2] = viewMatrix[2][2];

        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        glm::mat4 finalModelMatrix = glm::scale(
            glm::rotate(
                modelViewMatrix,
                glm::radians(particle->getRotation()),
                glm::vec3(0.0f, 0.0f, 1.0f)
            ),
            glm::vec3(particle->getScale())
        );

        m_transformationMatrixUniform->set(projectionMatrix * finalModelMatrix);
        m_lifetimeUniform->set(particle->getLifetime());

        m_model->bind();

        m_texture->bindActive(0);

        m_model->draw();

        m_texture->unbindActive(0);

        m_model->unbind();

        m_particleRenderingProgram->release();

        ::glDisable(GL_BLEND);
        ::glDepthMask(true);
    }

private:
    std::unique_ptr<globjects::Program> m_particleRenderingProgram;
    globjects::Uniform<glm::mat4>* m_transformationMatrixUniform;
    globjects::Uniform<float>* m_lifetimeUniform;

    std::unique_ptr<globjects::Shader> m_particleRenderingVertexShader;
    std::unique_ptr<globjects::Shader> m_particleRenderingFragmentShader;

    std::unique_ptr<Model> m_model;
    std::unique_ptr<globjects::Texture> m_texture;
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

    sf::Window window(videoMode, "Hello, Particles!", sf::Style::Default, settings);

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

    auto particleEmitter = std::make_unique<SimpleParticleEmitter>(5.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, 0.01f);
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

        const auto title = std::format("Hello, Particles! [frame render time, sec: {}]", deltaTime);

        window.setTitle(title);

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

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

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

        particleSystem->update(deltaTime);
        particleSystem->draw(cameraProjection, cameraView);

        // done rendering the frame

        window.display();
    }

    return 0;
}
