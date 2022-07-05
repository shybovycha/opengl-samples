#include "StaticModel.hpp"

StaticModel::StaticModel(std::vector<std::unique_ptr<StaticMesh>> meshes) :
    m_meshes(std::move(meshes)), m_transformation(1.0f)
{
}

void StaticModel::draw()
{
    for (auto& mesh : m_meshes)
    {
        mesh->draw();
    }
}

void StaticModel::drawInstanced(unsigned int instances)
{
    for (auto& mesh : m_meshes)
    {
        mesh->drawInstanced(instances);
    }
}

void StaticModel::bind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->bind();
    }
}

void StaticModel::unbind()
{
    for (auto& mesh : m_meshes)
    {
        mesh->unbind();
    }
}

void StaticModel::setTransformation(glm::mat4 transformation)
{
    // TODO: propagate onto meshes?
    m_transformation = transformation;
}

glm::mat4 StaticModel::getTransformation() const
{
    return m_transformation;
}
