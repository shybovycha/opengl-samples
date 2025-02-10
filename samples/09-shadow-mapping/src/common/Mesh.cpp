#include "Mesh.hpp"

Mesh::Mesh(
    std::unique_ptr<globjects::VertexArray> vao,
    std::vector<std::unique_ptr<globjects::Texture>> textures,
    std::vector<glm::vec3> vertices,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> uvs,
    std::vector<unsigned int> indices,
    std::unique_ptr<globjects::Buffer> vertexBuffer,
    std::unique_ptr<globjects::Buffer> indexBuffer,
    std::unique_ptr<globjects::Buffer> normalBuffer,
    std::unique_ptr<globjects::Buffer> uvBuffer) :

    m_vao(std::move(vao)),
    m_textures(std::move(textures)),
    m_vertices(std::move(vertices)),
    m_indices(std::move(indices)),
    m_uvs(std::move(uvs)),
    m_normals(std::move(normals)),
    m_vertexBuffer(std::move(vertexBuffer)),
    m_indexBuffer(std::move(indexBuffer)),
    m_normalBuffer(std::move(normalBuffer)),
    m_uvBuffer(std::move(uvBuffer))
{
}

Mesh::~Mesh()
{
}

std::unique_ptr<Mesh> Mesh::fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths)
{
    std::cout << "[INFO] Creating buffer objects...";

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

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

    std::vector<GLuint> indices;

    for (auto i = 0; i < mesh->mNumFaces; ++i)
    {
        auto& face = mesh->mFaces[i];

        for (auto t = 0; t < face.mNumIndices; ++t)
        {
            indices.push_back(face.mIndices[t]);
        }
    }

    auto vertexBuffer = std::make_unique<globjects::Buffer>();

    vertexBuffer->setData(vertices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    auto indexBuffer = std::make_unique<globjects::Buffer>();

    indexBuffer->setData(indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    auto vao = std::make_unique<globjects::VertexArray>();

    vao->bindElementBuffer(indexBuffer.get());

    vao->binding(0)->setAttribute(0);
    vao->binding(0)->setBuffer(vertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
    vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    vao->enable(0);

    auto normalBuffer = std::make_unique<globjects::Buffer>();

    if (!normals.empty())
    {
        normalBuffer->setData(normals, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        vao->binding(1)->setAttribute(1);
        vao->binding(1)->setBuffer(normalBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        vao->binding(1)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        vao->enable(1);

        // TODO: set uniform flag signalling the normals are present
    }

    auto uvBuffer = std::make_unique<globjects::Buffer>();

    if (!uvs.empty())
    {
        uvBuffer->setData(uvs, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        vao->binding(2)->setAttribute(2);
        vao->binding(2)->setBuffer(uvBuffer.get(), 0, sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
        vao->binding(2)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        vao->enable(2);

        // TODO: set uniform flag signalling the uvs are present
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading textures...";

    std::vector<std::unique_ptr<globjects::Texture>> textures;

    if (mesh->mMaterialIndex >= 0)
    {
        auto material = scene->mMaterials[mesh->mMaterialIndex];

        for (auto i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
        {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, i, &str);

            std::string imagePath { str.C_Str() };

            // TODO: extract the "std::string resolveFile(std::string)" helper
            /* std::vector<std::filesystem::path> lookupPaths = {
                imagePath,
                std::filesystem::path{ "../" + imagePath }
            };*/

            for (auto& path : materialLookupPaths)
            {
                const auto& filePath = std::filesystem::current_path() / path / imagePath;

                std::cout << "[INFO] Looking up the DIFFUSE texture in " << path << " (" << filePath << ")...";

                if (std::filesystem::exists(filePath))
                {
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

            auto texture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

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

            textures.push_back(std::move(texture));
        }

        // TODO: also handle aiTextureType_DIFFUSE and aiTextureType_SPECULAR
    }

    std::cout << "done" << std::endl;

    return std::make_unique<Mesh>(
        std::move(vao),
        std::move(textures),
        std::move(vertices),
        std::move(normals),
        std::move(uvs),
        std::move(indices),
        std::move(vertexBuffer),
        std::move(indexBuffer),
        std::move(normalBuffer),
        std::move(uvBuffer));
}

void Mesh::draw()
{
    // number of values passed = number of elements * number of vertices per element
    // in this case: 2 triangles, 3 vertex indexes per triangle
    m_vao->drawElements(
        static_cast<gl::GLenum>(GL_TRIANGLES),
        m_indices.size(),
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr);
}

void Mesh::drawInstanced(unsigned int instances)
{
    m_vao->drawElementsInstanced(
        static_cast<gl::GLenum>(GL_TRIANGLES),
        m_indices.size(),
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr,
        instances);
}

void Mesh::bind()
{
    m_vao->bind();

    for (auto& texture : m_textures)
    {
        texture->bindActive(1);
    }
}

void Mesh::unbind()
{
    for (auto& texture : m_textures)
    {
        texture->unbindActive(1);
    }

    m_vao->unbind();
}
