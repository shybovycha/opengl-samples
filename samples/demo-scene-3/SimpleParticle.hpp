#pragma once

#include <map>
#include <memory>
#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Model.hpp"
#include "Particle.hpp"

class SimpleParticle : public Particle
{
public:
    SimpleParticle();

    float getMass() const;

    float getRotation() const;

    glm::mat4 getModelMatrix() const override;

    void setMass(float mass);

    void setRotation(float rotation);

private:
    float m_mass;
    float m_rotation;
};


class SimpleParticleEmitter : public AbstractParticleEmitter<SimpleParticle>
{
public:
    SimpleParticleEmitter(std::shared_ptr<AbstractParticleParamsGenerator> paramsGenerator);

    void emit(SimpleParticle* particle) override;
};

class SimpleParticleAffector : public AbstractParticleAffector<SimpleParticle>
{
public:
    const glm::vec3 GRAVITY { 0.0f, -9.8f, 0.0f };

    void affect(SimpleParticle* particle, float deltaTime) override;

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
    SimpleParticleRenderer(std::unique_ptr<Model> model, std::unique_ptr<globjects::Texture> texture);

    void draw(SimpleParticle* particle, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) override;

private:
    std::unique_ptr<globjects::Program> m_particleRenderingProgram;
    globjects::Uniform<glm::mat4>* m_transformationMatrixUniform;
    globjects::Uniform<float>* m_lifetimeUniform;

    std::unique_ptr<globjects::Shader> m_particleRenderingVertexShader;
    std::unique_ptr<globjects::Shader> m_particleRenderingFragmentShader;

    std::unique_ptr<Model> m_model;
    std::unique_ptr<globjects::Texture> m_texture;
};
