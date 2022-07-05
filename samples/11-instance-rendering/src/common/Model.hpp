#pragma once

#include "stdafx.hpp"

#include "Mesh.hpp"

class Model
{
public:
    Model(std::vector<std::unique_ptr<Mesh>> meshes);

    ~Model();

    static std::unique_ptr<Model> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {});

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

    void draw();

    void drawInstanced(unsigned int instances);

    void bind();

    void unbind();

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<Mesh>>& meshes);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    glm::mat4 m_transformation;
};
