#include "Model.hpp"

Model::Model(std::vector<std::unique_ptr<Mesh>> meshes) :
    m_meshes(std::move(meshes)),
    m_transformation(1.0f)
{
}

Model::~Model()
{
}

std::unique_ptr<Model> Model::fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths)
{
    ZoneScopedN("Model::fromAiNode");

    std::vector<std::unique_ptr<Mesh>> meshes;

    processAiNode(scene, node, materialLookupPaths, meshes);

    return std::make_unique<Model>(std::move(meshes));
}

void Model::setTransformation(glm::mat4 transformation)
{
    ZoneScopedN("Model#setTransformation");

    m_transformation = transformation;
}

glm::mat4 Model::getTransformation() const
{
    return m_transformation;
}

void Model::draw()
{
    ZoneScopedN("Model#draw");

    for (auto& mesh : m_meshes)
    {
        mesh->draw();
    }
}

void Model::drawInstanced(unsigned int instances)
{
    ZoneScopedN("Model#drawInstanced");

    for (auto& mesh : m_meshes)
    {
        mesh->drawInstanced(instances);
    }
}

void Model::bind()
{
    ZoneScopedN("Model#bind");

    for (auto& mesh : m_meshes)
    {
        mesh->bind();
    }
}

void Model::unbind()
{
    ZoneScopedN("Model#unbind");

    for (auto& mesh : m_meshes)
    {
        mesh->unbind();
    }
}

void Model::processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<Mesh>>& meshes)
{
    ZoneScopedN("Model::processAiNode");

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
