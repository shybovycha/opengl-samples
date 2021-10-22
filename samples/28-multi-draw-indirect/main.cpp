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

struct StaticMesh
{
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
};

class AssimpStaticModelLoader
{
public:
    AssimpStaticModelLoader()
    {
    }

    static std::vector<StaticMesh> fromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = 0)
    {
        static auto importer = std::make_unique<Assimp::Importer>();
        auto scene = importer->ReadFile(filename, assimpImportFlags);

        std::vector<StaticMesh> meshes;

        if (!scene)
        {
            std::cerr << "failed: " << importer->GetErrorString() << std::endl;
            return meshes;
        }

        fromAiNode(scene, scene->mRootNode, materialLookupPaths, meshes);

        return meshes;
    }

protected:
    static void fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<StaticMesh>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths, meshes);
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];

            fromAiNode(scene, child, materialLookupPaths, meshes);
        }
    }

    static void fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths, std::vector<StaticMesh>& meshes)
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

        meshes.push_back({ .vertexPositions = vertices,
                           .normals = normals,
                           .uvs = uvs,
                           .indices = indices });
    }
};

std::shared_ptr<globjects::Program> loadShader(std::string vertexShaderFile, std::string fragmentShaderFile)
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

    auto program = std::make_shared<globjects::Program>();

    program->attach(vertexShader.get(), fragmentShader.get());

    program->link();

    if (!program->isLinked())
    {
        std::cerr << "Failed to link program" << std::endl;
        return nullptr;
    }

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
    settings.majorVersion = 4;
    settings.minorVersion = 6;
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

    auto duckScene = AssimpStaticModelLoader::fromFile("media/duck.obj", { "media" });
    auto lanternScene = AssimpStaticModelLoader::fromFile("media/lantern.obj", { "media" });
    auto scrollScene = AssimpStaticModelLoader::fromFile("media/scroll.obj", { "media" });
    auto penScene = AssimpStaticModelLoader::fromFile("media/pen.obj", { "media" });

    std::cout << "[INFO] Convert 3D models to static data..." << std::endl;

    std::vector<StaticMesh> meshes;
    std::map<unsigned int, StaticGeometryData> m_objectDataMap;

    meshes.insert(meshes.end(), std::make_move_iterator(duckScene.begin()), std::make_move_iterator(duckScene.end()));
    meshes.insert(meshes.end(), std::make_move_iterator(lanternScene.begin()), std::make_move_iterator(lanternScene.end()));
    meshes.insert(meshes.end(), std::make_move_iterator(scrollScene.begin()), std::make_move_iterator(scrollScene.end()));
    meshes.insert(meshes.end(), std::make_move_iterator(penScene.begin()), std::make_move_iterator(penScene.end()));

    m_objectDataMap[1] = {
        .transformation = glm::translate(glm::vec3(0.0f, 0.5f, 0.0f)),
        .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    };

    m_objectDataMap[2] = {
        .transformation = glm::translate(glm::vec3(0.0f, 0.5f, -0.5f)),
        .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
    };

    m_objectDataMap[3] = {
        .transformation = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f)),
        .color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    std::vector<unsigned int> m_indices;

    std::vector<StaticGeometryDrawCommand> m_drawCommands;

    std::vector<StaticGeometryData> m_objectData;

    struct NormalizedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    std::vector<NormalizedVertex> normalizedVertexData;
    unsigned int baseVertex = 0;
    unsigned int objectID = 0;

    for (auto mesh : meshes)
    {
        const auto numVertices = mesh.vertexPositions.size();

        for (size_t i = 0; i < numVertices; ++i)
        {
            auto position = mesh.vertexPositions[i];
            auto normal = mesh.normals[i];
            auto uv = mesh.uvs[i];

            normalizedVertexData.push_back({ .position = position, .normal = normal, .uv = uv });
        }

        m_indices.insert(m_indices.end(), mesh.indices.begin(), mesh.indices.end());

        StaticGeometryDrawCommand drawCommand {
            .elementCount = static_cast<unsigned int>(mesh.indices.size()),
            .instanceCount = 1,
            .firstIndex = 0,
            .baseVertex = baseVertex,
            .baseInstance = 0
        };

        m_drawCommands.push_back(drawCommand);

        StaticGeometryData objectData
        {
            .transformation = glm::mat4(),
            .color = glm::vec4(1.0f)
        };

        if (m_objectDataMap.find(objectID) != m_objectDataMap.end())
        {
            objectData = m_objectDataMap[objectID];
        }

        m_objectData.push_back(objectData);

        baseVertex += numVertices;
        ++objectID;
    }

    auto vao = std::make_unique<globjects::VertexArray>();

    // generate draw command buffer
    auto drawCommandBuffer = std::make_unique<globjects::Buffer>();
    drawCommandBuffer->setData(m_drawCommands, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW)); // draw commands can technically be changed

    // generate vertex data buffer
    auto geometryDataBuffer = std::make_unique<globjects::Buffer>();
    geometryDataBuffer->setData(normalizedVertexData, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    vao->binding(0)->setAttribute(0);
    vao->binding(0)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, position), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
    vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    vao->enable(0);

    vao->binding(1)->setAttribute(1);
    vao->binding(1)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, normal), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
    vao->binding(1)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    vao->enable(1);

    vao->binding(2)->setAttribute(2);
    vao->binding(2)->setBuffer(geometryDataBuffer.get(), offsetof(NormalizedVertex, uv), sizeof(NormalizedVertex)); // number of elements in buffer, stride, size of buffer element
    vao->binding(2)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    vao->enable(2);

    // generate element buffer
    auto elementBuffer = std::make_unique<globjects::Buffer>();
    elementBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

    vao->bindElementBuffer(elementBuffer.get());

    // generate object data buffer
    auto objectDataBuffer = std::make_unique<globjects::Buffer>();

    objectDataBuffer->setData(m_objectData, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

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

        drawCommandBuffer->bind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        objectDataBuffer->bindBase(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);
        
        vao->bind();

        vao->multiDrawElementsIndirect(static_cast<gl::GLenum>(GL_TRIANGLES), static_cast<gl::GLenum>(GL_UNSIGNED_INT), 0, m_drawCommands.size(), 0);

        simpleProgram->release();

        drawCommandBuffer->unbind(static_cast<gl::GLenum>(GL_DRAW_INDIRECT_BUFFER));
        objectDataBuffer->unbind(static_cast<gl::GLenum>(GL_SHADER_STORAGE_BUFFER), 4);
        
        vao->unbind();

        window.display();
    }

    return 0;
}
