#include "AbstractParticleParamsGenerator.hpp"

float AbstractParticleParamsGenerator::generateLifetime()
{
    return generateFloat(m_minLifetime, m_maxLifetime);
}

glm::vec3 AbstractParticleParamsGenerator::generateVelocity()
{
    auto velocityScale = generateFloat(m_minVelocity, m_maxVelocity);

    glm::vec3 velocityOffset;

    while (glm::dot(velocityOffset, m_velocityDirection) <= 0.0f)
    {
        velocityOffset = generateUnitVector();
    }

    return velocityOffset * velocityScale;
}

float AbstractParticleParamsGenerator::generateScale()
{
    return generateFloat(m_minScale, m_maxScale);
}

float AbstractParticleParamsGenerator::generateMass()
{
    return generateFloat(m_minMass, m_maxMass);
}

float AbstractParticleParamsGenerator::generateRotation()
{
    auto angle = generateFloat(m_minRotation, m_maxRotation);

    return glm::radians(angle);
}

glm::vec3 AbstractParticleParamsGenerator::generatePosition()
{
    glm::vec3 offset = generateUnitVector();

    auto offsetScale = generateFloat(m_minPositionOffset, m_maxPositionOffset);

    return m_origin + offset * offsetScale;
}

float AbstractParticleParamsGenerator::getMinLifetime() const
{
    return m_minLifetime;
}

float AbstractParticleParamsGenerator::getMaxLifetime() const
{
    return m_minLifetime;
}

float AbstractParticleParamsGenerator::getMinMass() const
{
    return m_minMass;
}

float AbstractParticleParamsGenerator::getMaxMass() const
{
    return m_minMass;
}

float AbstractParticleParamsGenerator::getMinRotation() const
{
    return m_minRotation;
}

float AbstractParticleParamsGenerator::getMaxRotation() const
{
    return m_minRotation;
}

float AbstractParticleParamsGenerator::getMinScale() const
{
    return m_minScale;
}

float AbstractParticleParamsGenerator::getMaxScale() const
{
    return m_minScale;
}

float AbstractParticleParamsGenerator::getMinVelocity() const
{
    return m_minVelocity;
}

float AbstractParticleParamsGenerator::getMaxVelocity() const
{
    return m_minVelocity;
}

glm::vec3 AbstractParticleParamsGenerator::getVelocityDirection() const
{
    return m_velocityDirection;
}

float AbstractParticleParamsGenerator::getMinPositionOffset() const
{
    return m_minPositionOffset;
}

float AbstractParticleParamsGenerator::getMaxPositionOffset() const
{
    return m_minPositionOffset;
}

glm::vec3 AbstractParticleParamsGenerator::getOrigin() const
{
    return m_origin;
}

void AbstractParticleParamsGenerator::setLifetimeInterval(float minLifetime, float maxLifetime)
{
    m_minLifetime = minLifetime;
    m_maxLifetime = maxLifetime;
}

void AbstractParticleParamsGenerator::setMassInterval(float minMass, float maxMass)
{
    m_minMass = minMass;
    m_maxMass = maxMass;
}

void AbstractParticleParamsGenerator::setRotationInterval(float minRotation, float maxRotation)
{
    m_minRotation = minRotation;
    m_maxRotation = maxRotation;
}

void AbstractParticleParamsGenerator::setScaleInterval(float minScale, float maxScale)
{
    m_minScale = minScale;
    m_maxScale = maxScale;
}

void AbstractParticleParamsGenerator::setVelocityInterval(float minVelocity, float maxVelocity, glm::vec3 velocityDirection)
{
    m_minVelocity = minVelocity;
    m_maxVelocity = maxVelocity;
    m_velocityDirection = velocityDirection;
}

void AbstractParticleParamsGenerator::setPositionInterval(float minPositionOffset, float maxPositionOffset, glm::vec3 origin)
{
    m_minPositionOffset = minPositionOffset;
    m_maxPositionOffset = maxPositionOffset;
    m_origin = origin;
}
