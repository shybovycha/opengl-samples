#include "Skybox.hpp"

CubemapSkyboxBuilder::CubemapSkyboxBuilder(std::unique_ptr<globjects::Texture> texture) :
    m_texture(texture.get()), AbstractSkyboxBuilder()
{
}

CubemapSkyboxBuilder::CubemapSkyboxBuilder(globjects::Texture* texture) :
    m_texture(texture), AbstractSkyboxBuilder()
{
}

globjects::Texture* CubemapSkyboxBuilder::getTexture()
{
    return m_texture;
}
