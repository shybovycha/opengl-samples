#include "SimpleParticle.hpp"

#ifdef WIN32
using namespace gl;
#endif

SimpleParticle::SimpleParticle() :
    Particle(), m_mass(0.0f), m_rotation(0.0f)
{
}

float SimpleParticle::getMass() const
{
    return m_mass;
}

float SimpleParticle::getRotation() const
{
    return m_rotation;
}

glm::mat4 SimpleParticle::getModelMatrix() const
{
    return glm::translate(
        glm::rotate(
            glm::scale(glm::mat4(1.0f), glm::vec3(getScale())),
            glm::radians(m_rotation),
            glm::vec3(0.0f, 0.0f, 1.0f)),
        getPosition());
}

void SimpleParticle::setMass(float mass)
{
    m_mass = mass;
}

void SimpleParticle::setRotation(float rotation)
{
    m_rotation = rotation;
}

SimpleParticleEmitter::SimpleParticleEmitter(std::shared_ptr<AbstractParticleParamsGenerator> paramsGenerator) :
    AbstractParticleEmitter(paramsGenerator)
{
}

void SimpleParticleEmitter::emit(SimpleParticle* particle)
{
    particle->setLifetime(m_paramsGenerator->generateLifetime());
    particle->setPosition(m_paramsGenerator->generatePosition());
    particle->setVelocity(m_paramsGenerator->generateVelocity());
    particle->setMass(m_paramsGenerator->generateMass());
    particle->setRotation(m_paramsGenerator->generateRotation());
    particle->setScale(m_paramsGenerator->generateScale());
}

void SimpleParticleAffector::affect(SimpleParticle* particle, float deltaTime)
{
    float speed = bezier<3>(particle->getLifetime(), 0.32f, 0.0f, 1.0f, 0.12f);

    particle->setLifetime(particle->getLifetime() - deltaTime);
    particle->setVelocity(particle->getVelocity() + speed * particle->getMass() * GRAVITY * deltaTime);
    particle->setPosition(particle->getPosition() + particle->getVelocity() * deltaTime);
    particle->setRotation(particle->getRotation() + 2.0f * deltaTime);
}

SimpleParticleRenderer::SimpleParticleRenderer(std::unique_ptr<Model> model, std::unique_ptr<globjects::Texture> texture) :
    m_model(std::move(model)), m_texture(std::move(texture))
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

void SimpleParticleRenderer::draw(SimpleParticle* particle, glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
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
            glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::vec3(particle->getScale()));

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
