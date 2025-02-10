#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "AbstractParticleParamsGenerator.hpp"

class Particle
{
public:
    Particle();

    float getLifetime() const;

    float getScale() const;

    glm::vec3 getPosition() const;

    glm::vec3 getVelocity() const;

    bool isAlive() const;

    virtual glm::mat4 getModelMatrix() const;

    void setLifetime(float lifetime);

    void setScale(float scale);

    void setPosition(glm::vec3 position);

    void setVelocity(glm::vec3 velocity);

    void kill();

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_scale;
    float m_lifetime;
};

template <class TParticle>
class AbstractParticleEmitter
{
public:
    AbstractParticleEmitter(std::shared_ptr<AbstractParticleParamsGenerator> paramsGenerator) :
        m_paramsGenerator(paramsGenerator)
    {
    }

    virtual void emit(TParticle* particle) = 0;

protected:
    std::shared_ptr<AbstractParticleParamsGenerator> m_paramsGenerator;
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
        std::shared_ptr<AbstractParticleEmitter<TParticle>> emitter,
        std::vector<std::shared_ptr<AbstractParticleAffector<TParticle>>> affectors,
        std::shared_ptr<AbstractParticleRenderer<TParticle>> renderer) :
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

    std::shared_ptr<AbstractParticleEmitter<TParticle>> m_emitter;
    std::shared_ptr<AbstractParticleRenderer<TParticle>> m_renderer;
    std::vector<std::shared_ptr<AbstractParticleAffector<TParticle>>> m_affectors;
};
