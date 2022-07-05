#pragma once

#include "stdafx.hpp"

#include "SingleMeshModel.hpp"
#include "CubemapSkyboxBuilder.hpp"
#include "SimpleSkyboxBuilder.hpp"

class Skybox : public SingleMeshModel
{
    friend class SimpleSkyboxBuilder;

public:
    static SimpleSkyboxBuilder* builder();

    static CubemapSkyboxBuilder* fromCubemap(globjects::Texture* cubemapTexture);

    Skybox(std::unique_ptr<AbstractMesh> mesh);
};
