#include "Skybox.hpp"

SimpleSkyboxBuilder* Skybox::builder()
{
    return new SimpleSkyboxBuilder();
}

CubemapSkyboxBuilder* Skybox::fromCubemap(globjects::Texture* cubemapTexture)
{
    return new CubemapSkyboxBuilder(cubemapTexture);
}

Skybox::Skybox(std::unique_ptr<StaticMesh> mesh) :
    StaticMesh(std::move(mesh))
{
}
