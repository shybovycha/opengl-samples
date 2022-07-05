#include "AbstractMesh.hpp"

std::shared_ptr<AbstractMeshBuilder> AbstractMesh::builder()
{
    return std::make_shared<AbstractMeshBuilder>();
}

AbstractMesh::AbstractMesh(
    std::vector<glm::vec3> vertices,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec3> tangents,
    std::vector<glm::vec3> bitangents,
    std::vector<glm::vec2> uvs,
    std::vector<unsigned int> indices,
    std::vector<globjects::Texture*> textures,
    std::unique_ptr<globjects::VertexArray> vao,
    std::unique_ptr<globjects::Buffer> vertexBuffer,
    std::unique_ptr<globjects::Buffer> indexBuffer,
    std::unique_ptr<globjects::Buffer> normalBuffer,
    std::unique_ptr<globjects::Buffer> tangentBuffer,
    std::unique_ptr<globjects::Buffer> bitangentBuffer,
    std::unique_ptr<globjects::Buffer> uvBuffer) :

    m_vertices(std::move(vertices)),
    m_indices(std::move(indices)),
    m_uvs(std::move(uvs)),
    m_normals(std::move(normals)),
    m_tangents(std::move(tangents)),
    m_bitangents(std::move(bitangents)),
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
        m_indices.size(),
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr);
}

void AbstractMesh::drawInstanced(unsigned int instances)
{
    m_vao->drawElementsInstanced(
        static_cast<gl::GLenum>(GL_TRIANGLES),
        m_indices.size(),
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
