#pragma once

#include "stdafx.hpp"

#include "SingleMeshModel.hpp"

class AbstractSkyboxBuilder
{
    friend class Skybox;

public:
    AbstractSkyboxBuilder* size(float size);

    std::unique_ptr<Skybox> build();

protected:
    virtual globjects::Texture* getTexture() = 0;

    float m_size = 1.0f;
};

class CubemapSkyboxBuilder : public AbstractSkyboxBuilder
{
public:
    CubemapSkyboxBuilder(std::unique_ptr<globjects::Texture> texture);

    CubemapSkyboxBuilder(globjects::Texture* texture);

protected:
    globjects::Texture* getTexture() override;

private:
    globjects::Texture* m_texture;
};

class SimpleSkyboxBuilder : public AbstractSkyboxBuilder
{
public:
    SimpleSkyboxBuilder();

    SimpleSkyboxBuilder* top(std::string filename);

    SimpleSkyboxBuilder* bottom(std::string filename);

    SimpleSkyboxBuilder* left(std::string filename);

    SimpleSkyboxBuilder* right(std::string filename);

    SimpleSkyboxBuilder* front(std::string filename);

    SimpleSkyboxBuilder* back(std::string filename);

protected:
    globjects::Texture* getTexture() override;

private:
    sf::Image m_top, m_bottom, m_left, m_right, m_front, m_back;
};

class Skybox : public SingleMeshModel
{
    friend class SimpleSkyboxBuilder;

public:
    static SimpleSkyboxBuilder* builder();

    static CubemapSkyboxBuilder* fromCubemap(globjects::Texture* cubemapTexture);

    Skybox(std::unique_ptr<AbstractMesh> mesh);
};
