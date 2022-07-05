#include "Skybox.hpp"

SimpleSkyboxBuilder* Skybox::builder()
{
    return new SimpleSkyboxBuilder();
}

CubemapSkyboxBuilder* Skybox::fromCubemap(globjects::Texture* cubemapTexture)
{
    return new CubemapSkyboxBuilder(cubemapTexture);
}

Skybox::Skybox(std::unique_ptr<AbstractMesh> mesh) :
    SingleMeshModel(std::move(mesh))
{
}
