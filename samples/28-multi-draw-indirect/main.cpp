#include <filesystem>
#include <iostream>
#include <sstream>
#include <random>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/TextureHandle.h>
#include <globjects/Uniform.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_relational.hpp>
#include <glm/ext/quaternion_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
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

struct StaticGeometryDrawCommand
{
    unsigned int elementCount; // number of elements (triangles) to be rendered for this object
    unsigned int instanceCount; // number of object instances
    unsigned int firstIndex; // offset into GL_ELEMENT_ARRAY_BUFFER
    unsigned int baseVertex; // offset of the first object' vertex in the uber-static-object-buffer
    unsigned int baseInstance; // offset of the first instance' per-instance-vertex-attributes; attribute index is calculated as: (gl_InstanceID / glVertexAttribDivisor()) + baseInstance
};

struct alignas(16) StaticObjectData
{
    glm::vec2 albedoTextureSize;
    glm::vec2 normalTextureSize;
    glm::vec2 emissionTextureSize;
    unsigned int instanceDataOffset;
};

struct alignas(16) StaticObjectInstanceData
{
    glm::mat4 transformation;
};

struct StaticMesh
{
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
};

struct StaticScene
{
    std::vector<StaticMesh> meshes;
    std::unique_ptr<sf::Image> albedoTexture;
    std::unique_ptr<sf::Image> normalTexture;
    std::unique_ptr<sf::Image> emissionTexture;
};

class AssimpStaticModelLoader
{
public:
    AssimpStaticModelLoader()
    {
    }

    static std::shared_ptr<StaticScene> fromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = aiProcess_Triangulate)
    {
        static auto importer = std::make_unique<Assimp::Importer>();
        auto scene = importer->ReadFile(filename, assimpImportFlags);

        auto resultScene = std::make_shared<StaticScene>();

        if (!scene)
        {
            std::cerr << "failed: " << importer->GetErrorString() << std::endl;
            return resultScene;
        }

        fromAiNode(scene, scene->mRootNode, materialLookupPaths, resultScene);

        return std::move(resultScene);
    }

protected:
    static void fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::shared_ptr<StaticScene> resultScene)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths, resultScene);
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];

            fromAiNode(scene, child, materialLookupPaths, resultScene);
        }
    }

    static std::unique_ptr<sf::Image> loadTexture(std::string& imagePath, std::vector<std::filesystem::path> materialLookupPaths)
    {
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
            return nullptr;
        }

        textureImage->flipVertically();

        return std::move(textureImage);
    }

    static void fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths, std::shared_ptr<StaticScene> resultScene)
    {
        std::cout << "[INFO] Creating buffer objects...";

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
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

        std::vector<std::unique_ptr<globjects::Texture>> textures;

        if (mesh->mMaterialIndex >= 0)
        {
            auto material = scene->mMaterials[mesh->mMaterialIndex];

            auto numDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
            auto numNormalTextures = material->GetTextureCount(aiTextureType_NORMALS);
            auto numEmissionTextures = material->GetTextureCount(aiTextureType_EMISSIVE);

            auto numEmissionColorTextures = material->GetTextureCount(aiTextureType_EMISSION_COLOR);
            auto numSpecularTextures = material->GetTextureCount(aiTextureType_SPECULAR);

            if (numDiffuseTextures > 0)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

                std::string imagePath { str.C_Str() };

                auto texture = loadTexture(imagePath, materialLookupPaths);

                resultScene->albedoTexture = std::move(texture);
            }

            if (numNormalTextures > 0)
            {
                aiString str;
                material->GetTexture(aiTextureType_NORMALS, 0, &str);

                std::string imagePath { str.C_Str() };

                auto texture = loadTexture(imagePath, materialLookupPaths);

                resultScene->normalTexture = std::move(texture);
            }

            if (numEmissionTextures > 0)
            {
                aiString str;
                material->GetTexture(aiTextureType_EMISSIVE, 0, &str);

                std::string imagePath { str.C_Str() };

                auto texture = loadTexture(imagePath, materialLookupPaths);

                resultScene->emissionTexture = std::move(texture);
            }
        }

        std::cout << "done" << std::endl;

        resultScene->meshes.push_back({ .vertexPositions = vertices,
                                        .normals = normals,
                                        .uvs = uvs,
                                        .indices = indices });
    }
};

class StaticGeometryDrawable
{
public:
    StaticGeometryDrawable() :
        m_vao(std::make_unique<globjects::VertexArray>()),
        m_drawCommandBuffer(std::make_unique<globjects::Buffer>()),
        m_geometryDataBuffer(std::make_unique<globjects::Buffer>()),
        m_elementBuffer(std::make_unique<globjects::Buffer>()),
        m_objectDataBuffer(std::make_unique<globjects::Buffer>()),
        m_objectInstanceDataBuffer(std::make_unique<globjects::Buffer>()),
        albedoTextures(std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D_ARRAY))),
        normalTextures(std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D_ARRAY))),
        emissionTextures(std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D_ARRAY)))
    {
    }

    ~StaticGeometryDrawable()
    {
        // albedoTextures->textureHandle().makeNonResident();
        // normalTextures->textureHandle().makeNonResident();
        // emissionTextures->textureHandle().makeNonResident();
    }

    void addScene(std::string sceneName, std::shared_ptr<StaticScene> scene)
    {
        StaticObjectData objectData {
            .albedoTextureSize = getTextureSize(scene->albedoTexture.get()),
            .normalTextureSize = getTextureSize(scene->normalTexture.get()),
            .emissionTextureSize = getTextureSize(scene->emissionTexture.get()),
            .instanceDataOffset = static_cast<unsigned int>(m_scenes.size())
        };

        m_scenes[sceneName] = {
            .scene = std::move(scene),
            .objectData = objectData,
            .instanceData = {}
        };
    }

    /*StaticSceneDescriptor getSceneInstance(std::string sceneName)
    {
        return m_scenes[sceneName];
    }*/

    void addSceneInstance(std::string sceneName, StaticObjectInstanceData instanceData)
    {
        if (m_scenes.find(sceneName) == m_scenes.end())
        {
            return;
        }

        m_scenes[sceneName].instanceData.push_back(instanceData);
    }

    void build()
    {
        std::vector<unsigned int> m_indices;

        m_drawCommands.clear();
        m_normalizedVertexData.clear();

        std::vector<StaticObjectData> m_objectData;
        std::vector<StaticObjectInstanceData> m_objectInstanceData;

        for (auto& sceneKV : m_scenes)
        {
            auto scene = sceneKV.second.scene;

            for (auto& mesh : scene->meshes)
            {
                unsigned int firstIndex = static_cast<unsigned int>(m_indices.size());
                unsigned int baseInstance = 0;
                unsigned int baseVertex = static_cast<unsigned int>(m_normalizedVertexData.size());

                unsigned int elementCount = 0;
                unsigned int vertexCount = 0;

                const auto numVertices = mesh.vertexPositions.size();

                for (size_t i = 0; i < numVertices; ++i)
                {
                    auto position = mesh.vertexPositions[i];
                    auto normal = mesh.normals[i];
                    auto uv = mesh.uvs[i];

                    m_normalizedVertexData.push_back({ .position = position, .normal = normal, .uv = uv });
                }

                m_indices.insert(m_indices.end(), mesh.indices.begin(), mesh.indices.end());

                elementCount += mesh.indices.size();
                vertexCount += numVertices;

                // if a scene contains multiple meshes, each of them will have their indexes reset (e.g. start from 0), so to count for that have to issue more draw commands
                StaticGeometryDrawCommand drawCommand {
                    .elementCount = static_cast<unsigned int>(mesh.indices.size()), // the number of elements to draw (triangles in this case)
                    .instanceCount = 1,
                    .firstIndex = firstIndex, // this is the offset of the index data in the unified index data buffer
                    .baseVertex = baseVertex, // this is the offset of vertex data sub-array within a big-big buffer of vertex data; essentially the first vertex' index of current object in unified vertex data buffer
                    .baseInstance = baseInstance
                };

                std::cout << "[DEBUG] Draw command: { "
                          << "elementCount: " << drawCommand.elementCount << "; "
                          << "instanceCount: " << drawCommand.instanceCount << "; "
                          << "firstIndex: " << drawCommand.firstIndex << "; "
                          << "baseVertex: " << drawCommand.baseVertex << "; "
                          << "baseInstance: " << drawCommand.baseInstance << " }"
                          << "\n";

                m_drawCommands.push_back(drawCommand);

                sceneKV.second.objectData.instanceDataOffset = m_objectInstanceData.size();

                // have to add redundant data to the objectDataBuffer and objectInstanceDataBuffer to count for multiple meshes within the same scene
                m_objectData.push_back(sceneKV.second.objectData); // TODO: the offset for the object/instance data here is incorrect
            }

            std::cout << "[DEBUG] End object " << sceneKV.first << "; instances to add: " << sceneKV.second.instanceData.size() << "\n";

            m_objectInstanceData.insert(m_objectInstanceData.end(), sceneKV.second.instanceData.begin(), sceneKV.second.instanceData.end());
        }

        std::cout << "[DEBUG] Object data: " << m_objectData.size() << "; instance data: " << m_objectInstanceData.size() << "\n";

        // generate draw command buffer
        m_drawCommandBuffer->setData(m_drawCommands, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW)); // draw commands can technically be changed

        // generate vertex data buffer
        m_geometryDataBuffer->setData(m_normalizedVertexData, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao->binding(0)->setAttribute(0);
        m_vao->binding(0)->setBuffer(m_geometryDataBuffer.get(), 0, sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(0);

        m_vao->binding(1)->setAttribute(1);
        m_vao->binding(1)->setBuffer(m_geometryDataBuffer.get(), offsetof(NormalizedVertex, normal), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(1)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(1);

        m_vao->binding(2)->setAttribute(2);
        m_vao->binding(2)->setBuffer(m_geometryDataBuffer.get(), offsetof(NormalizedVertex, uv), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(2)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(2);

        // generate element buffer
        m_elementBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        std::cout << "[DEBUG] Index buffer elements: " << m_indices.size() << "\n";

        m_vao->bindElementBuffer(m_elementBuffer.get());

        std::cout << "[DEBUG] Object data buffer elements: " << m_objectData.size() << "\n";

        m_objectDataBuffer->setData(m_objectData, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

        std::cout << "[DEBUG] Instance data buffer elements: " << m_objectInstanceData.size() << "\n";

        m_objectInstanceDataBuffer->setData(m_objectInstanceData, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

        // generate texture arrays
        glm::vec2 maxAlbedoTextureSize(0, 0);
        glm::vec2 maxNormalTextureSize(0, 0);
        glm::vec2 maxEmissionTextureSize(0, 0);

        for (auto& objectData : m_objectData)
        {
            maxAlbedoTextureSize.x = std::max(maxAlbedoTextureSize.x, objectData.albedoTextureSize.x);
            maxAlbedoTextureSize.y = std::max(maxAlbedoTextureSize.y, objectData.albedoTextureSize.y);

            maxNormalTextureSize.x = std::max(maxNormalTextureSize.x, objectData.normalTextureSize.x);
            maxNormalTextureSize.y = std::max(maxNormalTextureSize.y, objectData.normalTextureSize.y);

            maxEmissionTextureSize.x = std::max(maxEmissionTextureSize.x, objectData.emissionTextureSize.x);
            maxEmissionTextureSize.y = std::max(maxEmissionTextureSize.y, objectData.emissionTextureSize.y);
        }

        albedoTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        albedoTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        albedoTextures->image3D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec3(maxAlbedoTextureSize.x, maxAlbedoTextureSize.y, m_scenes.size()),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            nullptr);

        normalTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        normalTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        normalTextures->image3D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec3(maxNormalTextureSize.x, maxNormalTextureSize.y, m_scenes.size()),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            nullptr);

        emissionTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        emissionTextures->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        emissionTextures->image3D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec3(maxEmissionTextureSize.x, maxEmissionTextureSize.y, m_scenes.size()),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            nullptr);

        unsigned int i = 0;

        for (auto& sceneDescKV : m_scenes)
        {
            auto scene = sceneDescKV.second.scene;

            if (scene->albedoTexture != nullptr)
            {
                albedoTextures->subImage3D(
                    0,
                    glm::vec3(0, 0, i),
                    glm::vec3(scene->albedoTexture->getSize().x, scene->albedoTexture->getSize().y, 1),
                    static_cast<gl::GLenum>(GL_RGBA),
                    static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                    reinterpret_cast<const gl::GLvoid*>(scene->albedoTexture->getPixelsPtr()));
            }

            if (scene->normalTexture != nullptr)
            {
                normalTextures->subImage3D(
                    0,
                    glm::vec3(0, 0, i),
                    glm::vec3(scene->normalTexture->getSize().x, scene->normalTexture->getSize().y, 1),
                    static_cast<gl::GLenum>(GL_RGBA),
                    static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                    reinterpret_cast<const gl::GLvoid*>(scene->normalTexture->getPixelsPtr()));
            }

            if (scene->emissionTexture != nullptr)
            {
                emissionTextures->subImage3D(
                    0,
                    glm::vec3(0, 0, i),
                    glm::vec3(scene->emissionTexture->getSize().x, scene->emissionTexture->getSize().y, 1),
                    static_cast<gl::GLenum>(GL_RGBA),
                    static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                    reinterpret_cast<const gl::GLvoid*>(scene->emissionTexture->getPixelsPtr()));
            }

            ++i;
        }

        // albedoTextures->textureHandle().makeResident();
        // normalTextures->textureHandle().makeResident();
        // emissionTextures->textureHandle().makeResident();
    }

private:
    glm::vec2 getTextureSize(sf::Image* texture)
    {
        if (texture == nullptr)
        {
            return glm::vec2();
        }

        return glm::vec2(texture->getSize().x, texture->getSize().y);
    }

    struct StaticSceneDescriptor
    {
        std::shared_ptr<StaticScene> scene;
        StaticObjectData objectData;
        std::vector<StaticObjectInstanceData> instanceData;
    };

    struct alignas(16) NormalizedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    std::map<std::string, StaticSceneDescriptor> m_scenes;
    std::vector<NormalizedVertex> m_normalizedVertexData;

public:
    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_drawCommandBuffer;
    std::unique_ptr<globjects::Buffer> m_geometryDataBuffer;
    std::unique_ptr<globjects::Buffer> m_elementBuffer;
    std::unique_ptr<globjects::Buffer> m_objectDataBuffer;
    std::unique_ptr<globjects::Buffer> m_objectInstanceDataBuffer;

    std::vector<StaticGeometryDrawCommand> m_drawCommands;

    std::unique_ptr<globjects::Texture> albedoTextures;
    std::unique_ptr<globjects::Texture> normalTextures;
    std::unique_ptr<globjects::Texture> emissionTextures;
};

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 6;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;

#if defined(SYSTEM_DARWIN) || defined(HIGH_DPI)
    auto videoMode = sf::VideoMode(2048, 1536);
#else
    auto videoMode = sf::VideoMode(1024, 768);
#endif

    sf::Window window(videoMode, "Hello, Indirect drawing!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling simple vertex shader...";

    auto vertexSource = globjects::Shader::sourceFromFile("media/simple-rendering.vert");
    auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexSource.get());
    auto vertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

    if (!vertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader '" << "media/simple-rendering.vert" << "'" << std::endl;
        // return nullptr;
    }

    auto fragmentSource = globjects::Shader::sourceFromFile("media/simple-rendering.frag");
    auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentSource.get());
    auto fragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

    if (!fragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader '" << "media/simple-rendering.frag" << "'" << std::endl;
        // return nullptr;
    }

    auto simpleProgram = std::make_shared<globjects::Program>();
    simpleProgram->attach(vertexShader.get(), fragmentShader.get());

    simpleProgram->link();

    if (!simpleProgram->isLinked())
    {
        std::cerr << "Failed to link program" << std::endl;
        // return nullptr;
    }

    auto projectionUniform = simpleProgram->getUniform<glm::mat4>("projection");
    auto viewUniform = simpleProgram->getUniform<glm::mat4>("view");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D models..." << std::endl;

    auto inkBottleScene = AssimpStaticModelLoader::fromFile("media/ink-bottle.obj", { "media" });
    auto lanternScene = AssimpStaticModelLoader::fromFile("media/lantern.obj", { "media" });
    auto scrollScene = AssimpStaticModelLoader::fromFile("media/scroll.obj", { "media" });
    auto penScene = AssimpStaticModelLoader::fromFile("media/pen-lowpoly.obj", { "media" });
    auto tableScene = AssimpStaticModelLoader::fromFile("media/table.obj", { "media" });

    std::cout << "[INFO] Convert 3D models to static data..." << std::endl;

    auto staticDrawable = std::make_unique<StaticGeometryDrawable>();

    staticDrawable->addScene("inkBottle", inkBottleScene);
    staticDrawable->addScene("lantern", lanternScene);
    staticDrawable->addScene("scroll", scrollScene);
    staticDrawable->addScene("pen", penScene);
    staticDrawable->addScene("table", tableScene);

    staticDrawable->addSceneInstance("inkBottle", { .transformation = glm::translate(glm::vec3(-1.75f, 3.85f, 1.05f)) * glm::scale(glm::vec3(0.5f)) });

    staticDrawable->addSceneInstance("lantern", { .transformation = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f, 3.85f, -0.75f)), glm::vec3(0.5f)) });

    staticDrawable->addSceneInstance("scroll", { .transformation = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.85f, 0.0f)), glm::vec3(0.5f)) });

    staticDrawable->addSceneInstance("pen", {
        .transformation = 
            glm::translate(glm::vec3(0.35f, 3.95f, -0.75f)) * glm::scale(glm::vec3(0.05f)) * (glm::rotate(glm::radians(12.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
    });

    staticDrawable->addSceneInstance("table", { .transformation = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) });

    staticDrawable->build();

    // simpleProgram->setUniform("albedoTextures", staticDrawable->albedoTextures->textureHandle().handle());
    // simpleProgram->setUniform("normalTextures", staticDrawable->normalTextures->textureHandle().handle());
    // simpleProgram->setUniform("emissionTextures", staticDrawable->emissionTextures->textureHandle().handle());

    staticDrawable->albedoTextures->bindActive(1);
    staticDrawable->albedoTextures->bindActive(2);
    staticDrawable->albedoTextures->bindActive(3);

    simpleProgram->setUniform("albedoTextures", 1);
    simpleProgram->setUniform("normalTextures", 2);
    simpleProgram->setUniform("emissionTextures", 2);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 5.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    const float nearPlane = 0.1f;
    const float farPlane = 50.0f;

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

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y), 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.3f), static_cast<gl::GLfloat>(0.3f), static_cast<gl::GLfloat>(0.3f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

        projectionUniform->set(cameraProjection);
        viewUniform->set(cameraView);

        simpleProgram->use();

        staticDrawable->m_drawCommandBuffer->bind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        staticDrawable->m_objectDataBuffer->bindBase(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);
        staticDrawable->m_objectInstanceDataBuffer->bindBase(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 5);

        staticDrawable->m_vao->bind();

        ::glMultiDrawElementsIndirect(static_cast<gl::GLenum>(GL_TRIANGLES), static_cast<gl::GLenum>(GL_UNSIGNED_INT), nullptr, staticDrawable->m_drawCommands.size(), 0);

        // staticDrawable->m_vao->multiDrawElementsIndirect(static_cast<gl::GLenum>(GL_TRIANGLES), static_cast<gl::GLenum>(GL_UNSIGNED_INT), 0, staticDrawable->m_drawCommands.size(), 0);

        simpleProgram->release();

        staticDrawable->m_drawCommandBuffer->unbind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        staticDrawable->m_objectDataBuffer->unbind(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);
        staticDrawable->m_objectInstanceDataBuffer->unbind(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 5);

        staticDrawable->m_vao->unbind();

        window.display();
    }

    staticDrawable->albedoTextures->unbindActive(1);
    staticDrawable->albedoTextures->unbindActive(2);
    staticDrawable->albedoTextures->unbindActive(3);

    return 0;
}
