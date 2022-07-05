#include "AssimpStaticModelLoader.hpp"

AssimpStaticModelLoader::AssimpStaticModelLoader()
{
}

std::unique_ptr<StaticModel> AssimpStaticModelLoader::fromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths, unsigned int assimpImportFlags)
{
    static auto importer = std::make_unique<Assimp::Importer>();
    auto scene = importer->ReadFile(filename, assimpImportFlags);

    if (!scene)
    {
        std::cerr << "failed: " << importer->GetErrorString() << std::endl;
        return nullptr;
    }

    auto model = fromAiNode(scene, scene->mRootNode, materialLookupPaths);

    return std::move(model);
}

std::unique_ptr<StaticModel> AssimpStaticModelLoader::fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths)
{
    std::vector<std::unique_ptr<StaticMesh>> meshes;

    processAiNode(scene, node, materialLookupPaths, meshes);

    return std::make_unique<StaticModel>(std::move(meshes));
}

void AssimpStaticModelLoader::processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<StaticMesh>>& meshes)
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

std::unique_ptr<StaticMesh> AssimpStaticModelLoader::fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths)
{
    std::cout << "[INFO] Creating buffer objects...";

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> uvs;

    std::vector<GLuint> indices;

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

        if (mesh->HasTangentsAndBitangents())
        {
            glm::vec3 tangent(
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z);

            glm::vec3 bitangent(
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z);

            tangents.push_back(tangent);
            bitangents.push_back(bitangent);
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

            std::string imagePath { str.C_Str() };

            // TODO: extract the "std::string resolveFile(std::string)" helper
            /* std::vector<std::filesystem::path> lookupPaths = {
                imagePath,
                std::filesystem::path{ "../" + imagePath }
            };*/

            for (auto path : materialLookupPaths)
            {
                std::cout << "[INFO] Looking up the DIFFUSE texture in " << path << "...";

                const auto filePath = std::filesystem::path(path).append(imagePath);

                if (std::filesystem::exists(filePath))
                {
                    imagePath = filePath.string();
                    break;
                }
            }

            std::cout << "[INFO] Loading DIFFUSE texture " << imagePath << "...";

            auto textureImage = std::make_unique<sf::Image>();

            if (!textureImage->loadFromFile(imagePath))
            {
                std::cerr << "[ERROR] Can not load texture" << std::endl;
                continue;
            }

            textureImage->flipVertically();

            auto texture = new globjects::Texture(static_cast<gl::GLenum>(GL_TEXTURE_2D));

            texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
            texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

            texture->image2D(
                0,
                static_cast<gl::GLenum>(GL_RGBA8),
                glm::vec2(textureImage->getSize().x, textureImage->getSize().y),
                0,
                static_cast<gl::GLenum>(GL_RGBA),
                static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                reinterpret_cast<const gl::GLvoid*>(textureImage->getPixelsPtr()));

            textures.push_back(texture);
        }

        // TODO: also handle aiTextureType_DIFFUSE and aiTextureType_SPECULAR
    }

    std::cout << "done" << std::endl;

    return StaticMesh::builder()
        ->addVertices(vertices)
        ->addIndices(indices)
        ->addNormals(normals)
        ->addTangentsBitangents(tangents, bitangents)
        ->addUVs(uvs)
        ->addTextures(textures)
        ->build();
}
