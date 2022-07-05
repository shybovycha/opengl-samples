#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"

class AbstractMesh : public AbstractDrawable
{
    friend class AbstractMeshBuilder;

public:
    static std::shared_ptr<AbstractMeshBuilder> builder();

    AbstractMesh(
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
        std::unique_ptr<globjects::Buffer> uvBuffer);

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

protected:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;
    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_tangentBuffer;
    std::unique_ptr<globjects::Buffer> m_bitangentBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    std::vector<globjects::Texture*> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
    std::vector<glm::vec2> m_uvs;

    glm::mat4 m_transformation;
};
