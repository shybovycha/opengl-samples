#pragma once

#include "stdafx.hpp"

#include "AbstractSkyboxBuilder.hpp"

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
