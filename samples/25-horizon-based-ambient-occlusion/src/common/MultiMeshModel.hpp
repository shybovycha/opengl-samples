#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"
#include "AbstractMesh.hpp"

class MultiMeshModel : public AbstractDrawable
{
public:
    MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes);

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

protected:
    std::vector<std::unique_ptr<AbstractMesh>> m_meshes;
    glm::mat4 m_transformation;
};
