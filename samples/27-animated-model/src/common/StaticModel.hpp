#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"
#include "StaticMesh.hpp"

class StaticModel : public AbstractDrawable
{
public:
    StaticModel(std::vector<std::unique_ptr<StaticMesh>> meshes);

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

protected:
    std::vector<std::unique_ptr<StaticMesh>> m_meshes;
    glm::mat4 m_transformation;
};
