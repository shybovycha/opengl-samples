#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"

class StaticMesh : public AbstractDrawable
{
    friend class StaticMeshBuilder;

public:
    static std::shared_ptr<StaticMeshBuilder> builder();

    StaticMesh(
        size_t numIndices,
        std::vector<globjects::Texture*> textures,
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> vertexBuffer,
        std::unique_ptr<globjects::Buffer> indexBuffer,
        std::unique_ptr<globjects::Buffer> normalBuffer,
        std::unique_ptr<globjects::Buffer> tangentBuffer,
        std::unique_ptr<globjects::Buffer> bitangentBuffer,
        std::unique_ptr<globjects::Buffer> uvBuffer);

    StaticMesh(std::unique_ptr<StaticMesh> otherMesh);

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

    size_t m_numIndices;

    glm::mat4 m_transformation;
};

class StaticMeshBuilder
{
public:
    StaticMeshBuilder();

    StaticMeshBuilder* addVertices(std::vector<glm::vec3> vertices);

    StaticMeshBuilder* addIndices(std::vector<unsigned int> indices);

    StaticMeshBuilder* addNormals(std::vector<glm::vec3> normals);

    StaticMeshBuilder* addTangentsBitangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents);

    StaticMeshBuilder* addUVs(std::vector<glm::vec2> uvs);

    StaticMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture);

    StaticMeshBuilder* addTexture(globjects::Texture* texture);

    StaticMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures);

    StaticMeshBuilder* addTextures(std::vector<globjects::Texture*> textures);

    StaticMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex);

    StaticMeshBuilder* setTangentAttributerIndex(unsigned int tangentAttributeIndex);

    StaticMeshBuilder* setBitangentAttributerIndex(unsigned int bitangentAttributeIndex);

    StaticMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex);

    StaticMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex);

    std::unique_ptr<StaticMesh> build();

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_indices;
    std::vector<globjects::Texture*> m_textures;

    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_tangentBuffer;
    std::unique_ptr<globjects::Buffer> m_bitangentBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    unsigned int m_positionAttributeIndex;
    unsigned int m_normalAttributeIndex;
    unsigned int m_tangentAttributeIndex;
    unsigned int m_bitangentAttributeIndex;
    unsigned int m_uvAttributeIndex;
};
