#include "MultiMeshModel.hpp"

MultiMeshModel::MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) :
    m_meshes(std::move(meshes)), m_transformation(1.0f)
{
}

void MultiMeshModel::draw()
{
    for (auto& mesh : m_meshes)
    {
        mesh->draw();
    }
}

void MultiMeshModel::drawInstanced(unsigned int instances)
{
    for (auto& mesh : m_meshes)
    {
        mesh->drawInstanced(instances);
    }
}

void MultiMeshModel::bind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->bind();
    }
}

void MultiMeshModel::unbind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->unbind();
    }
}

void MultiMeshModel::setTransformation(glm::mat4 transformation)
{
    // TODO: propagate onto meshes?
    m_transformation = transformation;
}

glm::mat4 MultiMeshModel::getTransformation() const
{
    return m_transformation;
}
