#pragma once

#include "stdafx.hpp"

#include "AbstractMesh.hpp"
#include "Skybox.hpp"

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
