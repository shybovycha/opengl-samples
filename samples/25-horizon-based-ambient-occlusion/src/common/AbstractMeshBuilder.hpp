#pragma once

#include "stdafx.hpp"

#include "AbstractMesh.hpp"

class AbstractMeshBuilder
{
public:
    AbstractMeshBuilder();

    AbstractMeshBuilder* addVertices(std::vector<glm::vec3> vertices);

    AbstractMeshBuilder* addIndices(std::vector<unsigned int> indices);

    AbstractMeshBuilder* addNormals(std::vector<glm::vec3> normals);

    AbstractMeshBuilder* addTangentsBitangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents);

    AbstractMeshBuilder* addUVs(std::vector<glm::vec2> uvs);

    AbstractMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture);

    AbstractMeshBuilder* addTexture(globjects::Texture* texture);

    AbstractMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures);

    AbstractMeshBuilder* addTextures(std::vector<globjects::Texture*> textures);

    AbstractMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex);

    AbstractMeshBuilder* setTangentAttributerIndex(unsigned int tangentAttributeIndex);

    AbstractMeshBuilder* setBitangentAttributerIndex(unsigned int bitangentAttributeIndex);

    AbstractMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex);

    AbstractMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex);

    std::unique_ptr<AbstractMesh> build();

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
