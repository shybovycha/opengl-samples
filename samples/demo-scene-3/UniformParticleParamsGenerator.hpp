#pragma once

#include <random>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "AbstractParticleParamsGenerator.hpp"

class UniformParticleParamsGenerator : public AbstractParticleParamsGenerator
{
public:
    UniformParticleParamsGenerator();

protected:
    glm::vec3 generateUnitVector() override;

    float generateFloat(float minValue, float maxValue) override;

    unsigned int generateUInt(unsigned int minValue, unsigned int maxValue) override;

private:
    std::random_device m_randomDevice;
    std::default_random_engine m_generator;
};
