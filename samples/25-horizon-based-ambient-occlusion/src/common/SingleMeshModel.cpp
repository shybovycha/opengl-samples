#include "SingleMeshModel.hpp"

SingleMeshModel::SingleMeshModel(std::unique_ptr<AbstractMesh> mesh) :
    m_mesh(std::move(mesh)), m_transformation(1.0f)
{
}

void SingleMeshModel::draw()
{
    m_mesh->draw();
}

void SingleMeshModel::drawInstanced(unsigned int instances)
{
    m_mesh->drawInstanced(instances);
}

void SingleMeshModel::bind()
{
    m_mesh->bind();
}

void SingleMeshModel::unbind()
{
    m_mesh->unbind();
}

void SingleMeshModel::setTransformation(glm::mat4 transformation)
{
    m_transformation = transformation;
}

glm::mat4 SingleMeshModel::getTransformation() const
{
    return m_transformation;
}
