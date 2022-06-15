#include "Model.hpp"

Model::Model(std::vector<std::unique_ptr<Mesh>> meshes) :
    m_meshes(std::move(meshes)),
    m_transformation(1.0f)
{
}

std::unique_ptr<Model> Model::fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths)
{
    std::vector<std::unique_ptr<Mesh>> meshes;

    processAiNode(scene, node, materialLookupPaths, meshes);

    return std::make_unique<Model>(std::move(meshes));
}

void Model::setTransformation(glm::mat4 transformation)
{
    m_transformation = transformation;
}

glm::mat4 Model::getTransformation() const
{
    return m_transformation;
}

void Model::draw()
{
    for (auto& mesh : m_meshes)
    {
        mesh->draw();
    }
}

void Model::bind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->bind();
    }
}

void Model::unbind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->unbind();
    }
}

void Model::processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<Mesh>>& meshes)
{
    for (auto t = 0; t < node->mNumMeshes; ++t)
    {
        auto mesh = Mesh::fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
        meshes.push_back(std::move(mesh));
    }

    for (auto i = 0; i < node->mNumChildren; ++i)
    {
        auto child = node->mChildren[i];
        // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

        processAiNode(scene, child, materialLookupPaths, meshes);
    }
}
