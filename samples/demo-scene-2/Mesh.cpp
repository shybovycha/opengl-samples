#include "Mesh.hpp"

#pragma once

#include "AbstractDrawable.hpp"

std::shared_ptr<AbstractMeshBuilder> AbstractMesh::builder()
{
    return std::make_shared<AbstractMeshBuilder>();
}

AbstractMesh::AbstractMesh(
    unsigned int numIndices,
    std::vector<globjects::Texture*> textures,
    std::unique_ptr<globjects::VertexArray> vao,
    std::unique_ptr<globjects::Buffer> vertexBuffer,
    std::unique_ptr<globjects::Buffer> indexBuffer,
    std::unique_ptr<globjects::Buffer> normalBuffer,
    std::unique_ptr<globjects::Buffer> tangentBuffer,
    std::unique_ptr<globjects::Buffer> bitangentBuffer,
    std::unique_ptr<globjects::Buffer> uvBuffer
) :
    m_numIndices(numIndices),
    m_textures(textures),
    m_vao(std::move(vao)),
    m_vertexBuffer(std::move(vertexBuffer)),
    m_indexBuffer(std::move(indexBuffer)),
    m_normalBuffer(std::move(normalBuffer)),
    m_tangentBuffer(std::move(tangentBuffer)),
    m_bitangentBuffer(std::move(bitangentBuffer)),
    m_uvBuffer(std::move(uvBuffer)),
    m_transformation(1.0f)
{
}

void AbstractMesh::setTransformation(glm::mat4 transformation)
{
    m_transformation = transformation;
}

glm::mat4 AbstractMesh::getTransformation() const
{
    return m_transformation;
}

void AbstractMesh::draw()
{
    // number of values passed = number of elements * number of vertices per element
    // in this case: 2 triangles, 3 vertex indexes per triangle
    m_vao->drawElements(
        static_cast<gl::GLenum>(GL_TRIANGLES),
        m_numIndices,
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr);
}

void AbstractMesh::drawInstanced(unsigned int instances)
{
    m_vao->drawElementsInstanced(
        static_cast<gl::GLenum>(GL_TRIANGLES),
        m_numIndices,
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr,
        instances);
}

void AbstractMesh::bind()
{
    m_vao->bind();

    for (auto& texture : m_textures)
    {
        texture->bindActive(1);
    }
}

void AbstractMesh::unbind()
{
    for (auto& texture : m_textures)
    {
        texture->unbindActive(1);
    }

    m_vao->unbind();
}

AbstractMeshBuilder::AbstractMeshBuilder() :
    m_positionAttributeIndex(0),
    m_normalAttributeIndex(1),
    m_tangentAttributeIndex(3),
    m_bitangentAttributeIndex(4),
    m_uvAttributeIndex(2)
{
}

AbstractMeshBuilder* AbstractMeshBuilder::addVertices(std::vector<glm::vec3> vertices)
{
    m_vertices.insert(m_vertices.end(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.end()));

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addIndices(std::vector<unsigned int> indices)
{
    m_indices.insert(m_indices.end(), std::make_move_iterator(indices.begin()), std::make_move_iterator(indices.end()));

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addNormals(std::vector<glm::vec3> normals)
{
    m_normals.insert(m_normals.end(), std::make_move_iterator(normals.begin()), std::make_move_iterator(normals.end()));

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addTangentsBitangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents)
{
    m_tangents.insert(m_tangents.end(), std::make_move_iterator(tangents.begin()), std::make_move_iterator(tangents.end()));
    m_bitangents.insert(m_bitangents.end(), std::make_move_iterator(bitangents.begin()), std::make_move_iterator(bitangents.end()));

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addUVs(std::vector<glm::vec2> uvs)
{
    m_uvs.insert(m_uvs.end(), std::make_move_iterator(uvs.begin()), std::make_move_iterator(uvs.end()));

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addTexture(std::unique_ptr<globjects::Texture> texture)
{
    m_textures.push_back(texture.get());

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addTexture(globjects::Texture* texture)
{
    m_textures.push_back(texture);

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
{
    for (auto& texture : textures)
    {
        m_textures.push_back(texture.get());
    }

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::addTextures(std::vector<globjects::Texture*> textures)
{
    for (auto& texture : textures)
    {
        m_textures.push_back(texture);
    }

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::setPositionAttributerIndex(unsigned int positionAttributeIndex)
{
    m_positionAttributeIndex = positionAttributeIndex;

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::setTangentAttributerIndex(unsigned int tangentAttributeIndex)
{
    m_tangentAttributeIndex = tangentAttributeIndex;

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::setBitangentAttributerIndex(unsigned int bitangentAttributeIndex)
{
    m_bitangentAttributeIndex = bitangentAttributeIndex;

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::setNormalAttributerIndex(unsigned int normalAttributeIndex)
{
    m_normalAttributeIndex = normalAttributeIndex;

    return this;
}

AbstractMeshBuilder* AbstractMeshBuilder::setUVAttributerIndex(unsigned int uvAttributeIndex)
{
    m_uvAttributeIndex = uvAttributeIndex;

    return this;
}

std::unique_ptr<AbstractMesh> AbstractMeshBuilder::build()
{
    m_vertexBuffer = std::make_unique<globjects::Buffer>();

    m_vertexBuffer->setData(m_vertices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    m_indexBuffer = std::make_unique<globjects::Buffer>();

    m_indexBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    m_vao = std::make_unique<globjects::VertexArray>();

    m_vao->bindElementBuffer(m_indexBuffer.get());

    m_vao->binding(m_positionAttributeIndex)->setAttribute(m_positionAttributeIndex);
    m_vao->binding(m_positionAttributeIndex)->setBuffer(m_vertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
    m_vao->binding(m_positionAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    m_vao->enable(m_positionAttributeIndex);

    m_normalBuffer = std::make_unique<globjects::Buffer>();

    if (!m_normals.empty())
    {
        m_normalBuffer->setData(m_normals, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao->binding(m_normalAttributeIndex)->setAttribute(m_normalAttributeIndex);
        m_vao->binding(m_normalAttributeIndex)->setBuffer(m_normalBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_normalAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_normalAttributeIndex);

        // TODO: enable the corresponding shader sections (those which require normal data)
    }

    m_uvBuffer = std::make_unique<globjects::Buffer>();

    if (!m_uvs.empty())
    {
        m_uvBuffer->setData(m_uvs, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao->binding(m_uvAttributeIndex)->setAttribute(m_uvAttributeIndex);
        m_vao->binding(m_uvAttributeIndex)->setBuffer(m_uvBuffer.get(), 0, sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_uvAttributeIndex)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_uvAttributeIndex);

        // TODO: enable the corresponding shader sections (those which require UV data)
    }

    m_tangentBuffer = std::make_unique<globjects::Buffer>();

    if (!m_tangents.empty())
    {
        m_tangentBuffer->setData(m_tangents, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao->binding(m_tangentAttributeIndex)->setAttribute(m_tangentAttributeIndex);
        m_vao->binding(m_tangentAttributeIndex)->setBuffer(m_tangentBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_tangentAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_tangentAttributeIndex);

        // TODO: enable the corresponding shader sections (those which require tangent data)
    }

    m_bitangentBuffer = std::make_unique<globjects::Buffer>();

    if (!m_bitangents.empty())
    {
        m_bitangentBuffer->setData(m_bitangents, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao->binding(m_bitangentAttributeIndex)->setAttribute(m_bitangentAttributeIndex);
        m_vao->binding(m_bitangentAttributeIndex)->setBuffer(m_bitangentBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_bitangentAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_bitangentAttributeIndex);

        // TODO: enable the corresponding shader sections (those which require bitangent data)
    }

    return std::make_unique<AbstractMesh>(
        m_indices.size(),
        std::move(m_textures),
        std::move(m_vao),
        std::move(m_vertexBuffer),
        std::move(m_indexBuffer),
        std::move(m_normalBuffer),
        std::move(m_tangentBuffer),
        std::move(m_bitangentBuffer),
        std::move(m_uvBuffer));
}

SingleMeshModel::SingleMeshModel(std::unique_ptr<AbstractMesh> mesh) : m_mesh(std::move(mesh)), m_transformation(1.0f)
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

MultiMeshModel::MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) : m_meshes(std::move(meshes)), m_transformation(1.0f)
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
