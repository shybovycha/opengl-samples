#include "Particle.hpp"

Particle::Particle() :
    m_position(glm::vec3(0.0f)),
    m_velocity(glm::vec3(0.0f)),
    m_scale(1.0f),
    m_lifetime(1.0f)
{
}

float Particle::getLifetime() const
{
    return m_lifetime;
}

float Particle::getScale() const
{
    return m_scale;
}

glm::vec3 Particle::getPosition() const
{
    return m_position;
}

glm::vec3 Particle::getVelocity() const
{
    return m_velocity;
}

bool Particle::isAlive() const
{
    return m_lifetime > 0;
}

glm::mat4 Particle::getModelMatrix() const
{
    return glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(m_scale)), m_position);
}

void Particle::setLifetime(float lifetime)
{
    m_lifetime = lifetime;
}

void Particle::setScale(float scale)
{
    m_scale = scale;
}

void Particle::setPosition(glm::vec3 position)
{
    m_position = position;
}

void Particle::setVelocity(glm::vec3 velocity)
{
    m_velocity = velocity;
}

void Particle::kill()
{
    m_lifetime = 0.0f;
}
