#include <filesystem>
#include <iostream>
#include <sstream>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/Uniform.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#ifdef WIN32
using namespace gl;
#endif

class AbstractDrawable
{
public:
    virtual void draw() = 0;

    virtual void drawInstanced(unsigned int instances) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};

class AbstractMeshBuilder;

class AbstractMesh : public AbstractDrawable
{
    friend class AbstractMeshBuilder;

public:
    static std::shared_ptr<AbstractMeshBuilder> builder()
    {
        return std::make_shared<AbstractMeshBuilder>();
    }

    AbstractMesh(
        std::vector<glm::vec3> vertices,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> uvs,
        std::vector<unsigned int> indices,
        std::vector<globjects::Texture*> textures,
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> vertexBuffer,
        std::unique_ptr<globjects::Buffer> indexBuffer,
        std::unique_ptr<globjects::Buffer> normalBuffer,
        std::unique_ptr<globjects::Buffer> uvBuffer
    ) :

        m_vertices(std::move(vertices)),
        m_indices(std::move(indices)),
        m_uvs(std::move(uvs)),
        m_normals(std::move(normals)),
        m_textures(textures),
        m_vao(std::move(vao)),
        m_vertexBuffer(std::move(vertexBuffer)),
        m_indexBuffer(std::move(indexBuffer)),
        m_normalBuffer(std::move(normalBuffer)),
        m_uvBuffer(std::move(uvBuffer)),
        m_transformation(1.0f)
    {
    }

    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

    void draw() override
    {
        // number of values passed = number of elements * number of vertices per element
        // in this case: 2 triangles, 3 vertex indexes per triangle
        m_vao->drawElements(
            static_cast<gl::GLenum>(GL_TRIANGLES),
            m_indices.size(),
            static_cast<gl::GLenum>(GL_UNSIGNED_INT),
            nullptr);
    }

    void drawInstanced(unsigned int instances) override
    {
        m_vao->drawElementsInstanced(
            static_cast<gl::GLenum>(GL_TRIANGLES),
            m_indices.size(),
            static_cast<gl::GLenum>(GL_UNSIGNED_INT),
            nullptr,
            instances);
    }

    void bind() override
    {
        m_vao->bind();

        for (auto& texture : m_textures)
        {
            texture->bindActive(1);
        }
    }

    void unbind() override
    {
        for (auto& texture : m_textures)
        {
            texture->unbindActive(1);
        }

        m_vao->unbind();
    }

protected:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;
    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    std::vector<globjects::Texture*> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;

    glm::mat4 m_transformation;
};

class AbstractMeshBuilder
{
public:
    AbstractMeshBuilder() : m_positionAttributeIndex(0), m_normalAttributeIndex(1), m_uvAttributeIndex(2)
    {
    }

    AbstractMeshBuilder* addVertices(std::vector<glm::vec3> vertices)
    {
        m_vertices.insert(m_vertices.end(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.end()));

        return this;
    }

    AbstractMeshBuilder* addIndices(std::vector<unsigned int> indices)
    {
        m_indices.insert(m_indices.end(), std::make_move_iterator(indices.begin()), std::make_move_iterator(indices.end()));

        return this;
    }

    AbstractMeshBuilder* addNormals(std::vector<glm::vec3> normals)
    {
        m_normals.insert(m_normals.end(), std::make_move_iterator(normals.begin()), std::make_move_iterator(normals.end()));

        return this;
    }

    AbstractMeshBuilder* addUVs(std::vector<glm::vec2> uvs)
    {
        m_uvs.insert(m_uvs.end(), std::make_move_iterator(uvs.begin()), std::make_move_iterator(uvs.end()));

        return this;
    }

    AbstractMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture)
    {
        m_textures.push_back(texture.get());

        return this;
    }

    AbstractMeshBuilder* addTexture(globjects::Texture* texture)
    {
        m_textures.push_back(texture);

        return this;
    }

    AbstractMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        for (auto& texture : textures)
        {
            m_textures.push_back(texture.get());
        }

        return this;
    }

    AbstractMeshBuilder* addTextures(std::vector<globjects::Texture*> textures)
    {
        for (auto& texture : textures)
        {
            m_textures.push_back(texture);
        }

        return this;
    }

    AbstractMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex)
    {
        m_positionAttributeIndex = positionAttributeIndex;

        return this;
    }

    AbstractMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex)
    {
        m_normalAttributeIndex = normalAttributeIndex;

        return this;
    }

    AbstractMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex)
    {
        m_uvAttributeIndex = uvAttributeIndex;

        return this;
    }

    std::unique_ptr<AbstractMesh> build()
    {
        m_vertexBuffer = std::make_unique<globjects::Buffer>();

        m_vertexBuffer->setData(m_vertices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_indexBuffer = std::make_unique<globjects::Buffer>();

        m_indexBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao = std::make_unique<globjects::VertexArray>();

        m_vao->bindElementBuffer(m_indexBuffer.get());

        m_vao->binding(0)->setAttribute(0);
        m_vao->binding(0)->setBuffer(m_vertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(0);

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

        return std::make_unique<AbstractMesh>(
            m_vertices,
            m_normals,
            m_uvs,
            m_indices,
            std::move(m_textures),
            std::move(m_vao),
            std::move(m_vertexBuffer),
            std::move(m_indexBuffer),
            std::move(m_normalBuffer),
            std::move(m_uvBuffer));
    }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_indices;
    std::vector<globjects::Texture*> m_textures;

    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    unsigned int m_positionAttributeIndex;
    unsigned int m_normalAttributeIndex;
    unsigned int m_uvAttributeIndex;
};

class SingleMeshModel : public AbstractDrawable
{
public:
    SingleMeshModel(std::unique_ptr<AbstractMesh> mesh) : m_mesh(std::move(mesh)), m_transformation(1.0f)
    {
    }

    void draw() override
    {
        m_mesh->draw();
    }

    void drawInstanced(unsigned int instances) override
    {
        m_mesh->drawInstanced(instances);
    }

    void bind() override
    {
        m_mesh->bind();
    }

    void unbind() override
    {
        m_mesh->unbind();
    }

    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

protected:
    std::unique_ptr<AbstractMesh> m_mesh;
    glm::mat4 m_transformation;
};

class MultiMeshModel : public AbstractDrawable
{
public:
    MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) : m_meshes(std::move(meshes)), m_transformation(1.0f)
    {
    }

    void draw() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->draw();
        }
    }

    void drawInstanced(unsigned int instances) override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->drawInstanced(instances);
        }
    }

    void bind() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->bind();
        }
    }

    void unbind() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->unbind();
        }
    }

    void setTransformation(glm::mat4 transformation)
    {
        // TODO: propagate onto meshes?
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

protected:
    std::vector<std::unique_ptr<AbstractMesh>> m_meshes;
    glm::mat4 m_transformation;
};

class AssimpModel : public MultiMeshModel
{
public:
    AssimpModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) : MultiMeshModel(std::move(meshes))
    {
    }

    static std::unique_ptr<AssimpModel> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<std::unique_ptr<AbstractMesh>> meshes;

        processAiNode(scene, node, materialLookupPaths, meshes);

        return std::make_unique<AssimpModel>(std::move(meshes));
    }

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<AbstractMesh>>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
            meshes.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processAiNode(scene, child, materialLookupPaths, meshes);
        }
    }

    static std::unique_ptr<AbstractMesh> fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::cout << "[INFO] Creating buffer objects...";

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        std::vector<GLuint> indices;

        auto builder = AbstractMesh::builder();

        for (auto i = 0; i < mesh->mNumVertices; ++i)
        {
            glm::vec3 position(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);

            vertices.push_back(position);

            if (mesh->HasNormals())
            {
                glm::vec3 normal(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z);

                normals.push_back(normal);
            }

            if (mesh->HasTextureCoords(0))
            {
                glm::vec3 uv(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y,
                    mesh->mTextureCoords[0][i].z);

                uvs.push_back(uv);
            }
        }

        for (auto i = 0; i < mesh->mNumFaces; ++i)
        {
            auto face = mesh->mFaces[i];

            for (auto t = 0; t < face.mNumIndices; ++t)
            {
                indices.push_back(face.mIndices[t]);
            }
        }

        std::cout << "done" << std::endl;

        std::cout << "[INFO] Loading textures...";

        std::vector<globjects::Texture*> textures;

        if (mesh->mMaterialIndex >= 0)
        {
            auto material = scene->mMaterials[mesh->mMaterialIndex];

            for (auto i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, i, &str);

                std::string imagePath{ str.C_Str() };

                // TODO: extract the "std::string resolveFile(std::string)" helper
                /* std::vector<std::filesystem::path> lookupPaths = {
                    imagePath,
                    std::filesystem::path{ "../" + imagePath }
                };*/

                for (auto path : materialLookupPaths) {
                    std::cout << "[INFO] Looking up the DIFFUSE texture in " << path << "...";

                    const auto filePath = std::filesystem::path(path).append(imagePath);

                    if (std::filesystem::exists(filePath)) {
                        imagePath = filePath.string();
                        break;
                    }
                }

                std::cout << "[INFO] Loading DIFFUSE texture " << imagePath << "...";

                sf::Image textureImage;

                if (!textureImage.loadFromFile(imagePath))
                {
                    std::cerr << "[ERROR] Can not load texture" << std::endl;
                    continue;
                }

                textureImage.flipVertically();

                auto texture = new globjects::Texture(static_cast<gl::GLenum>(GL_TEXTURE_2D));

                texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
                texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

                texture->image2D(
                    0,
                    static_cast<gl::GLenum>(GL_RGBA8),
                    glm::vec2(textureImage.getSize().x, textureImage.getSize().y),
                    0,
                    static_cast<gl::GLenum>(GL_RGBA),
                    static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                    reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr()));

                textures.push_back(texture);
            }

            // TODO: also handle aiTextureType_DIFFUSE and aiTextureType_SPECULAR
        }

        std::cout << "done" << std::endl;

        return AbstractMesh::builder()
            ->addVertices(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->addTextures(textures)
            ->build();
    }
};

class Skybox;

class AbstractSkyboxBuilder
{
    friend class Skybox;

public:
    AbstractSkyboxBuilder* size(float size)
    {
        m_size = size;
        return this;
    }

    std::unique_ptr<Skybox> build()
    {
        std::vector<glm::vec3> vertices{
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, -1.0f },
        };

        std::vector<glm::vec3> normals{
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },

            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },

            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },

            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },

            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },

            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },

            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },

            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },

            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
        };

        std::for_each(vertices.begin(), vertices.end(), [this](glm::vec3 p) { return p * m_size; });

        std::vector<unsigned int> indices{
            2, 1, 0,
            2, 0, 3,
            4, 5, 6,
            7, 4, 6,
            10, 9, 8,
            10, 8, 11,
            12, 13, 14,
            15, 12, 14,
            18, 17, 16,
            18, 16, 19,
            20, 21, 22,
            23, 20, 22,
        };

        std::vector<glm::vec2> uvs{
            { 0.333333f, 0.500000f },
            { 0.333333f, 0.000000f },
            { 0.000000f, 0.000000f },
            { 0.000000f, 0.500000f },
            { 0.000000f, 1.000000f },
            { 0.000000f, 0.500000f },
            { 0.333333f, 0.500000f },
            { 0.333333f, 1.000000f },
            { 1.000000f, 1.000000f },
            { 1.000000f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 1.000000f },
            { 0.333333f, 1.000000f },
            { 0.333333f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 1.000000f },
            { 0.340000f, 0.500000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 0.000000f },
            { 0.340000f, 0.000000f },
            { 0.666666f, 0.500000f },
            { 0.666666f, 0.000000f },
            { 1.000000f, 0.000000f },
            { 1.000000f, 0.500000f },
        };

        auto mesh = AbstractMesh::builder()
            ->addVertices(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->addTexture(getTexture())
            ->build();

        return std::make_unique<Skybox>(std::move(mesh));
    }

protected:
    virtual globjects::Texture* getTexture() = 0;

    float m_size = 1.0f;
};

class CubemapSkyboxBuilder : public AbstractSkyboxBuilder
{
public:
    CubemapSkyboxBuilder(std::unique_ptr<globjects::Texture> texture) : m_texture(texture.get()), AbstractSkyboxBuilder(){}

    CubemapSkyboxBuilder(globjects::Texture* texture) : m_texture(texture), AbstractSkyboxBuilder(){}

protected:
    globjects::Texture* getTexture() override
    {
        return m_texture;
    }

private:
    globjects::Texture* m_texture;
};

class SimpleSkyboxBuilder : public AbstractSkyboxBuilder
{
public:
    SimpleSkyboxBuilder() : AbstractSkyboxBuilder() {}

    SimpleSkyboxBuilder* top(std::string filename)
    {
        m_top.loadFromFile(filename);
        return this;
    }

    SimpleSkyboxBuilder* bottom(std::string filename)
    {
        m_bottom.loadFromFile(filename);
        return this;
    }

    SimpleSkyboxBuilder* left(std::string filename)
    {
        m_left.loadFromFile(filename);
        return this;
    }

    SimpleSkyboxBuilder* right(std::string filename)
    {
        m_right.loadFromFile(filename);
        return this;
    }

    SimpleSkyboxBuilder* front(std::string filename)
    {
        m_front.loadFromFile(filename);
        return this;
    }

    SimpleSkyboxBuilder* back(std::string filename)
    {
        m_back.loadFromFile(filename);
        return this;
    }

protected:
    globjects::Texture* getTexture() override
    {
        std::map<gl::GLenum, sf::Image> skyboxTextures{
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X), m_right },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X), m_left },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y), m_top },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y), m_bottom },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z), m_back },
            { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z), m_front },
        };

        auto skyboxTexture = new globjects::Texture(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<GLint>(GL_CLAMP_TO_EDGE));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<GLint>(GL_CLAMP_TO_EDGE));
        skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<GLint>(GL_CLAMP_TO_EDGE));

        skyboxTexture->bind();

        for (auto& kv : skyboxTextures)
        {
            const auto target = kv.first;
            auto image = kv.second;

            if (target == gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y || target == gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
            {
                image.flipVertically();
            }
            else
            {
                image.flipHorizontally();
            }

            ::glTexImage2D(
                static_cast<::GLenum>(target),
                0,
                static_cast<::GLenum>(GL_RGBA8),
                static_cast<::GLsizei>(image.getSize().x),
                static_cast<::GLsizei>(image.getSize().y),
                0,
                static_cast<::GLenum>(GL_RGBA),
                static_cast<::GLenum>(GL_UNSIGNED_BYTE),
                reinterpret_cast<const ::GLvoid*>(image.getPixelsPtr()));
        }

        skyboxTexture->unbind();

        return skyboxTexture;
    }

private:
    sf::Image m_top, m_bottom, m_left, m_right, m_front, m_back;
};

class Skybox : public SingleMeshModel
{
    friend class SkyboxBuilder;

public:
    static SimpleSkyboxBuilder* builder()
    {
        return new SimpleSkyboxBuilder();
    }

    static CubemapSkyboxBuilder* fromCubemap(globjects::Texture* cubemapTexture)
    {
        return new CubemapSkyboxBuilder(cubemapTexture);
    }

    Skybox(std::unique_ptr<AbstractMesh> mesh) : SingleMeshModel(std::move(mesh))
    {
    }
};

struct alignas(16) PointLightData
{
    glm::vec3 lightPosition;
    float farPlane;
    std::array<glm::mat4, 6> projectionViewMatrices;
};

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 2;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;

#ifdef SYSTEM_DARWIN
    auto videoMode = sf::VideoMode(2048, 1536);
#else
    auto videoMode = sf::VideoMode(1024, 768);
#endif

    sf::Window window(videoMode, "Hello, Reflection!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling reflection mapping vertex shader...";

    auto reflectionMappingVertexSource = globjects::Shader::sourceFromFile("media/reflection-mapping.vert");
    auto reflectionMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingVertexSource.get());
    auto reflectionMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), reflectionMappingVertexShaderTemplate.get());

    if (!reflectionMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection mapping geometry shader...";

    auto reflectionMappingGeometrySource = globjects::Shader::sourceFromFile("media/reflection-mapping.geom");
    auto reflectionMappingGeometryShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingGeometrySource.get());
    auto reflectionMappingGeometryShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_GEOMETRY_SHADER), reflectionMappingGeometryShaderTemplate.get());

    if (!reflectionMappingGeometryShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection mapping fragment shader...";

    auto reflectionMappingFragmentSource = globjects::Shader::sourceFromFile("media/reflection-mapping.frag");
    auto reflectionMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionMappingFragmentSource.get());
    auto reflectionMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), reflectionMappingFragmentShaderTemplate.get());

    if (!reflectionMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection mapping fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking reflection mapping shaders..." << std::endl;

    auto reflectionMappingProgram = std::make_unique<globjects::Program>();
    reflectionMappingProgram->attach(reflectionMappingVertexShader.get(), reflectionMappingGeometryShader.get(), reflectionMappingFragmentShader.get());

    auto reflectionMappingModelTransformationUniform = reflectionMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple rendering vertex shader...";

    auto simpleRenderingVertexSource = globjects::Shader::sourceFromFile("media/simple-rendering.vert");
    auto simpleRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleRenderingVertexSource.get());
    auto simpleRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), simpleRenderingVertexShaderTemplate.get());

    if (!simpleRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple rendering mapping vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple rendering fragment shader...";

    auto simpleRenderingFragmentSource = globjects::Shader::sourceFromFile("media/simple-rendering.frag");
    auto simpleRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleRenderingFragmentSource.get());
    auto simpleRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), simpleRenderingFragmentShaderTemplate.get());

    if (!simpleRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking simple rendering shaders..." << std::endl;

    auto simpleRenderingProgram = std::make_unique<globjects::Program>();
    simpleRenderingProgram->attach(simpleRenderingVertexShader.get(), simpleRenderingFragmentShader.get());

    auto simpleRenderingModelTransformationUniform = simpleRenderingProgram->getUniform<glm::mat4>("model");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling skybox rendering vertex shader...";

    auto skyboxRenderingVertexSource = globjects::Shader::sourceFromFile("media/skybox.vert");
    auto skyboxRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingVertexSource.get());
    auto skyboxRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), skyboxRenderingVertexShaderTemplate.get());

    if (!skyboxRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point skybox rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling point skybox rendering fragment shader...";

    auto skyboxRenderingFragmentSource = globjects::Shader::sourceFromFile("media/skybox.frag");
    auto skyboxRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingFragmentSource.get());
    auto skyboxRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), skyboxRenderingFragmentShaderTemplate.get());

    if (!skyboxRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile point skybox rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking point skybox rendering shaders..." << std::endl;

    auto skyboxRenderingProgram = std::make_unique<globjects::Program>();
    skyboxRenderingProgram->attach(skyboxRenderingVertexShader.get(), skyboxRenderingFragmentShader.get());

    auto skyboxRenderingModelTransformationUniform = skyboxRenderingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection rendering vertex shader...";

    auto reflectionRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/reflection-rendering.vert");
    auto reflectionRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionRenderingVertexShaderSource.get());
    auto reflectionRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), reflectionRenderingVertexShaderTemplate.get());

    if (!reflectionRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling reflection rendering fragment shader...";

    auto reflectionRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/reflection-rendering.frag");
    auto reflectionRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(reflectionRenderingFragmentShaderSource.get());
    auto reflectionRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), reflectionRenderingFragmentShaderTemplate.get());

    if (!reflectionRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile reflection rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking reflection rendering shader...";

    auto reflectionRenderingProgram = std::make_unique<globjects::Program>();
    reflectionRenderingProgram->attach(reflectionRenderingVertexShader.get(), reflectionRenderingFragmentShader.get());

    auto reflectionRenderingModelTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("model");
    auto reflectionRenderingViewTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("view");
    auto reflectionRenderingProjectionTransformationUniform = reflectionRenderingProgram->getUniform<glm::mat4>("projection");

    // auto reflectionRenderingLightColorUniform = reflectionRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto reflectionRenderingCameraPositionUniform = reflectionRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto houseScene = importer.ReadFile("media/house1.obj", 0);

    if (!houseScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto houseModel = AssimpModel::fromAiNode(houseScene, houseScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    houseModel->setTransformation(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), glm::vec3(0.0f, 0.75f, 0.0f)));

    auto tableScene = importer.ReadFile("media/table.obj", 0);

    if (!tableScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto tableModel = AssimpModel::fromAiNode(tableScene, tableScene->mRootNode, { "media" });

    tableModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    auto lanternScene = importer.ReadFile("media/lantern.obj", 0);

    if (!lanternScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto lanternModel = AssimpModel::fromAiNode(lanternScene, lanternScene->mRootNode, { "media" });

    lanternModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f, 3.85f, -0.75f)), glm::vec3(0.5f)));

    // TODO: extract this to material class
    sf::Image lanternEmissionMapImage;

    if (!lanternEmissionMapImage.loadFromFile("media/lantern_emission.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternEmissionMapImage.flipVertically();

    auto lanternEmissionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternEmissionMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternEmissionMapImage.getSize().x, lanternEmissionMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternEmissionMapImage.getPixelsPtr()));

    // TODO: extract this to material class
    sf::Image lanternSpecularMapImage;

    if (!lanternSpecularMapImage.loadFromFile("media/lantern_specular.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternSpecularMapImage.flipVertically();

    auto lanternSpecularMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternSpecularMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternSpecularMapImage.getSize().x, lanternSpecularMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternSpecularMapImage.getPixelsPtr()));

    auto scrollScene = importer.ReadFile("media/scroll.obj", 0);

    if (!scrollScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto scrollModel = AssimpModel::fromAiNode(scrollScene, scrollScene->mRootNode, { "media" });

    scrollModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.85f, 0.0f)), glm::vec3(0.5f)));

    auto inkBottleScene = importer.ReadFile("media/ink-bottle.obj", 0);

    if (!inkBottleScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto inkBottleModel = AssimpModel::fromAiNode(inkBottleScene, inkBottleScene->mRootNode, { "media" });

    // TODO: encapsulate position and rotation in model class instead of just exposing transformation
    // this constant here is taken from ink bottle position
    glm::vec3 reflectiveModelPosition = glm::vec3(-1.75f, 3.85f, 1.05f);

    inkBottleModel->setTransformation(
        glm::translate(reflectiveModelPosition) *
        glm::scale(glm::vec3(0.5f))
    );

    auto penScene = importer.ReadFile("media/pen-lowpoly.obj", 0);

    if (!penScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto penModel = AssimpModel::fromAiNode(penScene, penScene->mRootNode, { "media" });

    // rotate -> scale -> translate; can be done as series of matrix multiplications M_translation * M_scale * M_rotation
    // each of the components, in turn, can also be a series of matrix multiplications: M_rotation = M_rotate_z * M_rotate_y * M_rotate_x
    penModel->setTransformation(
        glm::translate(glm::vec3(0.35f, 3.95f, -0.75f)) *
        glm::scale(glm::vec3(0.05f)) *
        (glm::rotate(glm::radians(12.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
    );

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing reflectionMapTexture...";

    auto reflectionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    reflectionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    reflectionMapTexture->bind();

    const auto reflectionMapSize = 2048;

    for (auto i = 0; i < 6; ++i)
    {
        ::glTexImage2D(
            static_cast<::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
            0,
            GL_RGBA8,
            reflectionMapSize,
            reflectionMapSize,
            0,
            GL_RGBA,
            GL_UNSIGNED_INT,
            nullptr);
    }

    reflectionMapTexture->unbind();

    auto reflectionMapDepthTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    reflectionMapDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    reflectionMapDepthTexture->bind();

    for (auto i = 0; i < 6; ++i)
    {
        ::glTexImage2D(
            static_cast<::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
            0,
            GL_DEPTH_COMPONENT,
            reflectionMapSize,
            reflectionMapSize,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
    }

    reflectionMapDepthTexture->unbind();

    /*auto skybox = Skybox::fromCubemap(reflectionMapTexture.get())
        ->size(40.0f)
        ->build();*/

    auto skybox = Skybox::builder()
        ->top("media/skybox-top.png")
        ->bottom("media/skybox-bottom.png")
        ->left("media/skybox-left.png")
        ->right("media/skybox-right.png")
        ->front("media/skybox-front.png")
        ->back("media/skybox-back.png")
        ->size(40.0f)
        ->build();

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing reflection mapping frame buffer...";

    auto reflectionMappingFramebuffer = std::make_unique<globjects::Framebuffer>();
    reflectionMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), reflectionMapTexture.get());
    reflectionMappingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), reflectionMapDepthTexture.get());

    // tell framebuffer it actually needs to render to **BOTH** textures, but does not have to output anywhere (last NONE argument, iirc)
    reflectionMappingFramebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });

    reflectionMappingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 5.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
        sf::Event event{};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

#ifdef WIN32
        if (!window.hasFocus())
        {
            continue;
        }
#endif

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

#ifdef WIN32
        glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));
        sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
#else
        glm::vec2 mouseDelta = currentMousePos - previousMousePos;
        previousMousePos = currentMousePos;
#endif

        float horizontalAngle = (mouseDelta.x / static_cast<float>(window.getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
        float verticalAngle = (mouseDelta.y / static_cast<float>(window.getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

        cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
        cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

        cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos += cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos -= cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
            }
        }

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        const float nearPlane = 0.1f;
        const float farPlane = 10.0f;

        // this is a cubemap, hence aspect ratio **must** be 1:1
        glm::mat4 reflectionProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

        // TODO: technically, this should be calculated as AABB / 2
        const auto reflectionOffset = glm::vec3(0.0f, 0.05f, 0.0f);

        reflectionMappingProgram->setUniform("projectionViewMatrices[0]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[1]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[2]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[3]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[4]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        reflectionMappingProgram->setUniform("projectionViewMatrices[5]", reflectionProjection * glm::lookAt(reflectiveModelPosition + reflectionOffset, reflectiveModelPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // first render pass - shadow mapping
        reflectionMappingFramebuffer->bind();

        ::glViewport(0, 0, reflectionMapSize, reflectionMapSize);
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_BACK);

        reflectionMappingProgram->use();

        reflectionMappingProgram->setUniform("diffuseTexture", 1);

        reflectionMappingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        reflectionMappingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        reflectionMappingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        reflectionMappingModelTransformationUniform->set(scrollModel->getTransformation());

        // scroll model needs culling to be disabled since this is a modified plane, so...
        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        reflectionMappingModelTransformationUniform->set(penModel->getTransformation());

        penModel->bind();
        penModel->draw();
        penModel->unbind();

        reflectionMappingFramebuffer->unbind();

        reflectionMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        skyboxRenderingProgram->use();
        skyboxRenderingProgram->setUniform("projection", cameraProjection);
        skyboxRenderingProgram->setUniform("view", glm::mat4(glm::mat3(cameraView)));

#ifndef _DEBUG
        reflectionMapTexture->bindActive(0);
        skyboxRenderingProgram->setUniform("cubeMap", 0);
#else
        skyboxRenderingProgram->setUniform("cubeMap", 1);
#endif

        skybox->bind();
        skybox->draw();
        skybox->unbind();

        reflectionMapTexture->unbindActive(0);

        skyboxRenderingProgram->release();

        // render object with reflective material

        reflectionRenderingProgram->use();

        reflectionRenderingProgram->setUniform("projection", cameraProjection);
        reflectionRenderingProgram->setUniform("view", cameraView);
        reflectionRenderingProgram->setUniform("model", inkBottleModel->getTransformation());

        reflectionRenderingProgram->setUniform("cameraPosition", cameraPos);

        reflectionMapTexture->bindActive(0);

        reflectionRenderingProgram->setUniform("reflectionMap", 0);
        reflectionRenderingProgram->setUniform("diffuseTexture", 1);

        inkBottleModel->bind();
        inkBottleModel->draw();
        inkBottleModel->unbind();

        reflectionMapTexture->unbindActive(0);

        reflectionRenderingProgram->release();

        // draw the scene

        simpleRenderingProgram->use();

        simpleRenderingProgram->setUniform("projection", cameraProjection);
        simpleRenderingProgram->setUniform("view", cameraView);

        simpleRenderingProgram->setUniform("diffuseTexture", 1);

        simpleRenderingModelTransformationUniform->set(houseModel->getTransformation());

#ifdef _DEBUG
        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();
#endif

        simpleRenderingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        simpleRenderingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        simpleRenderingModelTransformationUniform->set(penModel->getTransformation());

        penModel->bind();
        penModel->draw();
        penModel->unbind();

        simpleRenderingModelTransformationUniform->set(scrollModel->getTransformation());

        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        simpleRenderingProgram->release();

        // done rendering the frame

        window.display();
    }

    return 0;
}
