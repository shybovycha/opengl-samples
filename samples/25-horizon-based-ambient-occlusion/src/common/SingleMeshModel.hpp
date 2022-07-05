#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"
#include "AbstractMesh.hpp"

class SingleMeshModel : public AbstractDrawable
{
public:
    SingleMeshModel(std::unique_ptr<AbstractMesh> mesh);

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

protected:
    std::unique_ptr<AbstractMesh> m_mesh;
    glm::mat4 m_transformation;
};
