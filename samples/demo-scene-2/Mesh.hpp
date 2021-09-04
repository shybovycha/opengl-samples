#pragma once

#include "stdafx.hpp"

#include "AbstractDrawable.hpp"

class AbstractMeshBuilder;

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
        std::unique_ptr<globjects::Buffer> uvBuffer
    );

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

class SingleMeshModel : public AbstractDrawable
{
public:
    SingleMeshModel(std::unique_ptr<AbstractMesh> mesh);

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

protected:
    std::unique_ptr<AbstractMesh> m_mesh;
    glm::mat4 m_transformation;
};

class MultiMeshModel : public AbstractDrawable
{
public:
    MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes);

    void draw() override;

    void drawInstanced(unsigned int instances) override;

    void bind() override;

    void unbind() override;

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

protected:
    std::vector<std::unique_ptr<AbstractMesh>> m_meshes;
    glm::mat4 m_transformation;
};
