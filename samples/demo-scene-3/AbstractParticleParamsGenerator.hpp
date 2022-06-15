#pragma once

#include <random>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class AbstractRandomVectorGenerator
{
public:
    virtual glm::vec3 generateUnitVector() = 0;
};

class AbstractParticleParamsGenerator
{
public:
    virtual float generateLifetime();

    virtual glm::vec3 generateVelocity();

    virtual float generateScale();

    virtual float generateMass();

    virtual float generateRotation();

    virtual glm::vec3 generatePosition();

public:
    float getMinLifetime() const;

    float getMaxLifetime() const;

    float getMinMass() const;

    float getMaxMass() const;

    float getMinRotation() const;

    float getMaxRotation() const;

    float getMinScale() const;

    float getMaxScale() const;

    float getMinVelocity() const;

    float getMaxVelocity() const;

    glm::vec3 getVelocityDirection() const;

    float getMinPositionOffset() const;

    float getMaxPositionOffset() const;

    glm::vec3 getOrigin() const;

public:
    void setLifetimeInterval(float minLifetime, float maxLifetime);

    void setMassInterval(float minMass, float maxMass);

    void setRotationInterval(float minRotation, float maxRotation);

    void setScaleInterval(float minScale, float maxScale);

    void setVelocityInterval(float minVelocity, float maxVelocity, glm::vec3 velocityDirection);

    void setPositionInterval(float minPositionOffset, float maxPositionOffset, glm::vec3 origin);

protected:
    virtual glm::vec3 generateUnitVector() = 0;

    virtual float generateFloat(float minValue, float maxValue) = 0;

    virtual unsigned int generateUInt(unsigned int minValue, unsigned int maxValue) = 0;

private:
    float m_minLifetime;
    float m_maxLifetime;

    float m_minScale;
    float m_maxScale;

    float m_minRotation;
    float m_maxRotation;

    float m_minMass;
    float m_maxMass;

    float m_minVelocity;
    float m_maxVelocity;
    glm::vec3 m_velocityDirection;

    float m_minPositionOffset;
    float m_maxPositionOffset;
    glm::vec3 m_origin;
};
