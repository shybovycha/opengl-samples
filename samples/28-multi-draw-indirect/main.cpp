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

struct alignas(16) StaticGeometryData
{
    glm::mat4 transformation;
    glm::vec4 color;
    // std::vector<globjects::TextureHandle> textures;
};

class StaticGeometryDrawableBuilder;

class StaticGeometryBuilder;

class StaticGeometryDrawable
{
    friend class StaticGeometryDrawableBuilder;

public:
    std::shared_ptr<StaticGeometryDrawableBuilder> builder()
    {
        return std::make_shared<StaticGeometryDrawableBuilder>();
    }

    void bind()
    {
        m_drawCommandBuffer->bind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        m_objectDataBuffer->bindBase(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);

        m_vao->bind();
    }

    void unbind()
    {
        m_drawCommandBuffer->unbind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        m_objectDataBuffer->unbind(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);

        m_vao->unbind();
    }

    void draw()
    {
        m_vao->multiDrawElementsIndirect(static_cast<gl::GLenum>(GL_TRIANGLES), static_cast<gl::GLenum>(GL_UNSIGNED_INT), 0, m_drawCommands.size(), 0);
    }

// private:
    StaticGeometryDrawable(
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> geometryDataBuffer,
        std::unique_ptr<globjects::Buffer> elementBuffer,
        std::unique_ptr<globjects::Buffer> drawCommandBuffer,
        std::unique_ptr<globjects::Buffer> objectDataBuffer,
        std::vector<StaticGeometryData> objectData,
        std::vector<float> normalizedVertexData,
        std::vector<unsigned int> indices,
        std::vector<StaticGeometryDrawCommand> drawCommands,
        std::vector<std::unique_ptr<globjects::Texture>> textures) :

        m_vao(std::move(vao)),
        m_geometryDataBuffer(std::move(geometryDataBuffer)),
        m_elementBuffer(std::move(elementBuffer)),
        m_drawCommandBuffer(std::move(drawCommandBuffer)),

        m_objectDataBuffer(std::move(objectDataBuffer)),
        m_objectData(std::move(objectData)),

        m_normalizedVertexData(std::move(normalizedVertexData)),
        m_indices(std::move(indices)),
        m_drawCommands(std::move(drawCommands)),
        m_textures(std::move(textures))
    {
    }

private:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_geometryDataBuffer;
    std::unique_ptr<globjects::Buffer> m_elementBuffer;
    std::unique_ptr<globjects::Buffer> m_drawCommandBuffer;
    std::unique_ptr<globjects::Buffer> m_objectDataBuffer;

    std::vector<StaticGeometryData> m_objectData;

    std::vector<float> m_normalizedVertexData;
    std::vector<unsigned int> m_indices;
    std::vector<StaticGeometryDrawCommand> m_drawCommands;
    std::vector<std::unique_ptr<globjects::Texture>> m_textures;
};

class StaticGeometryDrawableBuilder
{
    friend class StaticGeometryBuilder;

public:
    std::unique_ptr<StaticGeometryDrawable> build()
    {
        // normalize the vertex data into one big chunk of memory
        // std::vector<float> normalizedVertexData;
        struct NormalizedVertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
        };

        std::vector<NormalizedVertex> normalizedVertexData;

        const auto numVertices = m_positions.size();

        for (size_t i = 0; i < numVertices; ++i)
        {
            auto position = m_positions[i];
            auto normal = m_normals[i];
            auto uv = m_uvs[i];

            /*normalizedVertexData.push_back(position.x);
            normalizedVertexData.push_back(position.y);
            normalizedVertexData.push_back(position.z);

            normalizedVertexData.push_back(normal.x);
            normalizedVertexData.push_back(normal.y);
            normalizedVertexData.push_back(normal.z);

            normalizedVertexData.push_back(uv.x);
            normalizedVertexData.push_back(uv.y);*/

            normalizedVertexData.push_back({ .position = position, .normal = normal, .uv = uv });
        }

        auto vao = std::make_unique<globjects::VertexArray>();

        // generate draw command buffer
        auto drawCommandBuffer = std::make_unique<globjects::Buffer>();
        drawCommandBuffer->setData(m_drawCommands, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW)); // draw commands can technically be changed

        // generate vertex data buffer
        auto geometryDataBuffer = std::make_unique<globjects::Buffer>();
        geometryDataBuffer->setData(normalizedVertexData, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        // since we have put (3 * float = position) + (3 * float = normal) + (2 * float = uv), we can use these values for stride and offset

        // position: base offset = 0 (first attribute), stride = 8
        vao->binding(0)->setAttribute(0);
        // vao->binding(0)->setBuffer(geometryDataBuffer.get(), 0, sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
        vao->binding(0)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, position), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        vao->enable(0);

        // normal: base offset = 3 (sizeof(position)), stride = 8 (sizeof(position) + sizeof(normal) + sizeof(uv))
        vao->binding(1)->setAttribute(1);
        // vao->binding(1)->setBuffer(geometryDataBuffer.get(), sizeof(glm::vec3), sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
        vao->binding(1)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, normal), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        vao->binding(1)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        vao->enable(1);

        // UV: base offset = 6 (sizeof(position) + sizeof(normal)), stride = 8 (sizeof(position) + sizeof(normal) + sizeof(uv))
        vao->binding(2)->setAttribute(2);
        // vao->binding(2)->setBuffer(geometryDataBuffer.get(), sizeof(glm::vec3) + sizeof(glm::vec3), sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
        vao->binding(2)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, uv), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
        vao->binding(2)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        vao->enable(2);

        // gl_InstanceID - technically, an object ID; used fromanother buffer
        vao->binding(3)->setAttribute(3);
        vao->binding(3)->setBuffer(drawCommandBuffer.get(), offsetof(StaticGeometryDrawCommand, baseInstance), sizeof(StaticGeometryDrawCommand));
        vao->binding(3)->setDivisor(1);
        vao->enable(3);

        // generate element buffer
        auto elementBuffer = std::make_unique<globjects::Buffer>();
        elementBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        vao->bindElementBuffer(elementBuffer.get());

        // make all textures resident in video memory
        for (auto& texture : m_textures)
        {
            texture->textureHandle().makeResident();
        }

        // generate object data buffer
        std::vector<StaticGeometryData> objectData;

        for (auto i = 0; i < m_drawCommands.size(); ++i)
        {
            objectData.push_back({});
        }

        for (auto kv : m_objectDataMap)
        {
            objectData[kv.first] = kv.second;
        }

        auto objectDataBuffer = std::make_unique<globjects::Buffer>();

        objectDataBuffer->setData(objectData, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

        return std::make_unique<StaticGeometryDrawable>(
            std::move(vao),
            std::move(geometryDataBuffer),
            std::move(elementBuffer),
            std::move(drawCommandBuffer),
            std::move(objectDataBuffer),
            std::move(objectData),
            std::move(normalizedVertexData),
            std::move(m_indices),
            std::move(m_drawCommands),
            std::move(m_textures));
    }

    std::shared_ptr<StaticGeometryBuilder> startMesh()
    {
        return std::make_shared<StaticGeometryBuilder>(this);
    }

    void setObjectTransformation(unsigned int objectID, glm::mat4 transformation)
    {
        if (m_objectDataMap.find(objectID) == m_objectDataMap.end())
        {
            m_objectDataMap[objectID] = {};
        }

        m_objectDataMap[objectID].transformation = transformation;
    }

    void setObjectColor(unsigned int objectID, glm::vec4 color)
    {
        if (m_objectDataMap.find(objectID) == m_objectDataMap.end())
        {
            m_objectDataMap[objectID] = {};
        }

        m_objectDataMap[objectID].color = color;
    }

    void setObjectTextures(unsigned int objectID, std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        if (m_objectDataMap.find(objectID) == m_objectDataMap.end())
        {
            m_objectDataMap[objectID] = {};
        }

        std::vector<globjects::TextureHandle> textureHandles;

        for (auto& texture : textures)
        {
            textureHandles.push_back(texture->textureHandle().handle());
        }

        /*std::transform(
            textures.begin(),
            textures.end(),
            textureHandles.begin(),
            [](std::unique_ptr<globjects::Texture> t) {
                return t->textureHandle().handle();
            });*/

        // m_objectDataMap[objectID].textures = textureHandles;
    }

// private:
    StaticGeometryDrawableBuilder()
    {
    }

    unsigned int addMesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices, std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        const auto instanceID = m_drawCommands.size();

        // generate new draw command for this mesh
        StaticGeometryDrawCommand drawCommand {
            .elementCount = static_cast<unsigned int>(indices.size()),
            .instanceCount = 1, // TODO: return a reference to this object to allow for instanced drawing?
            .firstIndex = 0,
            .baseVertex = static_cast<unsigned int>(m_positions.size()),
            .baseInstance = static_cast<unsigned int>(instanceID)
        };

        m_drawCommands.push_back(drawCommand);

        // TODO: reserve empty space or copy default values if any of the attributes is not present

        // copy mesh data to preserve the memory
        m_positions.insert(m_positions.end(), std::make_move_iterator(positions.begin()), std::make_move_iterator(positions.end()));
        m_normals.insert(m_normals.end(), std::make_move_iterator(normals.begin()), std::make_move_iterator(normals.end()));
        m_uvs.insert(m_uvs.end(), std::make_move_iterator(uvs.begin()), std::make_move_iterator(uvs.end()));
        m_indices.insert(m_indices.end(), std::make_move_iterator(indices.begin()), std::make_move_iterator(indices.end()));
        m_textures.insert(m_textures.end(), std::make_move_iterator(textures.begin()), std::make_move_iterator(textures.end()));

        return static_cast<unsigned int>(instanceID);
    }

private:
    std::vector<StaticGeometryDrawCommand> m_drawCommands;

    std::map<unsigned int, StaticGeometryData> m_objectDataMap;

    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_indices;

    std::vector<std::unique_ptr<globjects::Texture>> m_textures;
};

class StaticGeometryBuilder
{
public:
    StaticGeometryBuilder* addVertexPositions(std::vector<glm::vec3> positions)
    {
        // m_positions = positions;
        m_positions.insert(m_positions.end(), std::make_move_iterator(positions.begin()), std::make_move_iterator(positions.end()));

        return this;
    }

    StaticGeometryBuilder* addNormals(std::vector<glm::vec3> normals)
    {
        // m_normals = normals;
        m_normals.insert(m_normals.end(), std::make_move_iterator(normals.begin()), std::make_move_iterator(normals.end()));

        return this;
    }

    StaticGeometryBuilder* addUVs(std::vector<glm::vec2> uvs)
    {
        // m_uvs = uvs;
        m_uvs.insert(m_uvs.end(), std::make_move_iterator(uvs.begin()), std::make_move_iterator(uvs.end()));

        return this;
    }

    StaticGeometryBuilder* addIndices(std::vector<unsigned int> indices)
    {
        // m_indices = indices;
        m_indices.insert(m_indices.end(), std::make_move_iterator(indices.begin()), std::make_move_iterator(indices.end()));

        return this;
    }

    StaticGeometryBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        // m_textures = textures;
        m_textures.insert(m_textures.end(), std::make_move_iterator(textures.begin()), std::make_move_iterator(textures.end()));

        return this;
    }

    unsigned int endMesh()
    {
        return m_parent->addMesh(m_positions, m_normals, m_uvs, m_indices, std::move(m_textures));
    }

// private:
    StaticGeometryBuilder(StaticGeometryDrawableBuilder* parent) :
        m_parent(parent)
    {
    }

private:
    StaticGeometryDrawableBuilder* m_parent;

    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_indices;

    std::vector<std::unique_ptr<globjects::Texture>> m_textures;
};

class AssimpStaticModelLoader
{
public:
    AssimpStaticModelLoader()
    {
    }

    static unsigned int fromFile(StaticGeometryDrawableBuilder* builder, std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = 0)
    {
        static auto importer = std::make_unique<Assimp::Importer>();
        auto scene = importer->ReadFile(filename, assimpImportFlags);

        if (!scene)
        {
            std::cerr << "failed: " << importer->GetErrorString() << std::endl;
            return 0;
        }

        auto meshBuilder = builder->startMesh();

        fromAiNode(scene, scene->mRootNode, materialLookupPaths, meshBuilder.get());

        return meshBuilder->endMesh();
    }

protected:
    static void fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, StaticGeometryBuilder* meshBuilder)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths, meshBuilder);
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];

            fromAiNode(scene, child, materialLookupPaths, meshBuilder);
        }
    }

    static void fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths, StaticGeometryBuilder* meshBuilder)
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

            for (auto i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, i, &str);

                std::string imagePath { str.C_Str() };

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

                auto texture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

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

                textures.push_back(std::move(texture));
            }
        }

        std::cout << "done" << std::endl;

        meshBuilder
            ->addVertexPositions(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->addTextures(std::move(textures));
    }
};

std::unique_ptr<globjects::Program> loadShader(std::string vertexShaderFile, std::string fragmentShaderFile)
{
    auto vertexSource = globjects::Shader::sourceFromFile(vertexShaderFile);
    auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexSource.get());
    auto vertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

    if (!vertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader '" << vertexShaderFile << "'" << std::endl;
        return nullptr;
    }

    auto fragmentSource = globjects::Shader::sourceFromFile(fragmentShaderFile);
    auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentSource.get());
    auto fragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

    if (!fragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader '" << fragmentShaderFile << "'" << std::endl;
        return nullptr;
    }

    auto program = std::make_unique<globjects::Program>();

    program->attach(vertexShader.get(), fragmentShader.get());

    program->link();

    return std::move(program);
}

struct alignas(16) PointLightDescriptor
{
    glm::vec3 position;
    float strength;
    glm::vec4 color;
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

    auto simpleProgram = loadShader("media/simple-rendering.vert", "media/simple-rendering.frag");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    auto staticDrawableBuilder = std::make_unique<StaticGeometryDrawableBuilder>();

    auto quadModelID = AssimpStaticModelLoader::fromFile(staticDrawableBuilder.get(), "media/quad.obj", {}, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    // auto grassModelID = AssimpStaticModelLoader::fromFile(staticDrawableBuilder.get(), "media/grass3.obj", { "media" });

    // auto palmModelID = AssimpStaticModelLoader::fromFile(staticDrawableBuilder.get(), "media/palm.obj", { "media" });

    staticDrawableBuilder->setObjectTransformation(quadModelID, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -0.5f)));
    staticDrawableBuilder->setObjectColor(quadModelID, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // staticDrawableBuilder->setObjectColor(grassModelID, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    // staticDrawableBuilder->setObjectColor(palmModelID, glm::vec4(0.3f, 1.0f, 0.7f, 1.0f));

    auto staticGeometry = staticDrawableBuilder->build();

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 5.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    glm::vec3 lightPosition = glm::vec3(-28.0f, 7.0f, 0.0f); // cameraPos;

    const float nearPlane = 0.1f;
    const float farPlane = 50.0f;
    glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, nearPlane, farPlane);

    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

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

        simpleProgram->use();

        simpleProgram->setUniform("projection", cameraProjection);
        simpleProgram->setUniform("view", cameraView);

        staticGeometry->bind();
        staticGeometry->draw();
        staticGeometry->unbind();

        simpleProgram->release();

        window.display();
    }

    return 0;
}
