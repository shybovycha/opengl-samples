#include "UniformParticleParamsGenerator.hpp"

UniformParticleParamsGenerator::UniformParticleParamsGenerator() :
    m_generator(std::default_random_engine(m_randomDevice()))
{
}

glm::vec3 UniformParticleParamsGenerator::generateUnitVector()
{
    std::uniform_real_distribution<> distribution(-1.0f, 1.0f);

    auto x = distribution(m_generator);
    auto y = distribution(m_generator);
    auto z = distribution(m_generator);

    return glm::vec3(x, y, z);
}

float UniformParticleParamsGenerator::generateFloat(float minValue, float maxValue)
{
    std::uniform_real_distribution<> distribution(minValue, maxValue);

    return distribution(m_generator);
}

unsigned int UniformParticleParamsGenerator::generateUInt(unsigned int minValue, unsigned int maxValue)
{
    std::uniform_int_distribution<> distribution(minValue, maxValue);

    return static_cast<unsigned int>(distribution(m_generator));
}
