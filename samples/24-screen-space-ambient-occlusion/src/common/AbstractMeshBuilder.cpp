#include "AbstractMeshBuilder.hpp"

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
        m_vertices,
        m_normals,
        m_tangents,
        m_bitangents,
        m_uvs,
        m_indices,
        std::move(m_textures),
        std::move(m_vao),
        std::move(m_vertexBuffer),
        std::move(m_indexBuffer),
        std::move(m_normalBuffer),
        std::move(m_tangentBuffer),
        std::move(m_bitangentBuffer),
        std::move(m_uvBuffer));
}
