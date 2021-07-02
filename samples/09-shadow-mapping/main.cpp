#include <filesystem>
#include <iostream>
#include <sstream>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
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

class Mesh
{
public:
    Mesh(
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

    ~Mesh()
    {
    }

    static std::unique_ptr<Mesh> fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {})
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
            auto face = mesh->mFaces[i];

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

    void draw()
    {
        // number of values passed = number of elements * number of vertices per element
        // in this case: 2 triangles, 3 vertex indexes per triangle
        m_vao->drawElements(
            static_cast<gl::GLenum>(GL_TRIANGLES),
            m_indices.size(),
            static_cast<gl::GLenum>(GL_UNSIGNED_INT),
            nullptr);
    }

    void bind()
    {
        m_vao->bind();

        for (auto& texture : m_textures)
        {
            texture->bindActive(1);
        }
    }

    void unbind()
    {
        for (auto& texture : m_textures)
        {
            texture->unbindActive(1);
        }

        m_vao->unbind();
    }

private:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;
    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    std::vector<std::unique_ptr<globjects::Texture>> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
};

class Model
{
public:
    Model(std::vector<std::unique_ptr<Mesh>> meshes) :
        m_meshes(std::move(meshes)),
        m_transformation(1.0f)
    {
    }

    ~Model()
    {
    }

    static std::unique_ptr<Model> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<std::unique_ptr<Mesh>> meshes;

        processAiNode(scene, node, materialLookupPaths, meshes);

        return std::make_unique<Model>(std::move(meshes));
    }

    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

    void draw()
    {
        for (auto& mesh : m_meshes)
        {
            mesh->draw();
        }
    }

    void bind()
    {
        for (auto& mesh : m_meshes)
        {
            mesh->bind();
        }
    }

    void unbind()
    {
        for (auto& mesh : m_meshes)
        {
            mesh->unbind();
        }
    }

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<Mesh>>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = Mesh::fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
            meshes.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processAiNode(scene, child, materialLookupPaths, meshes);
        }
    }

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    glm::mat4 m_transformation;
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

    sf::Window window(videoMode, "Hello, Shadow mapping!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling shadow mapping vertex shader...";

    auto shadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping.vert");
    auto shadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingVertexSource.get());
    auto shadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowMappingVertexShaderTemplate.get());

    if (!shadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping geometry shader...";

    auto shadowMappingGeometrySource = globjects::Shader::sourceFromFile("media/shadow-mapping.geom");
    auto shadowMappingGeometryShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingGeometrySource.get());
    auto shadowMappingGeometryShader = std::make_unique<globjects::Shader>(gl::GL_GEOMETRY_SHADER, shadowMappingGeometryShaderTemplate.get());

    if (!shadowMappingGeometryShader->compile())
    {
        std::cerr << "[ERROR] Can not compile geometry shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping fragment shader...";

    auto shadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping.frag");
    auto shadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingFragmentSource.get());
    auto shadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowMappingFragmentShaderTemplate.get());

    if (!shadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating shadow mapping program...";

    auto shadowMappingProgram = std::make_unique<globjects::Program>();

    shadowMappingProgram->attach(shadowMappingVertexShader.get(), shadowMappingGeometryShader.get(), shadowMappingFragmentShader.get());

    auto shadowMappingModelTransformationUniform = shadowMappingProgram->getUniform<glm::mat4>("modelTransformation");
    auto shadowMappingLightViewProjectionMatrices = shadowMappingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjectionMatrix");
    auto lightViewProjectionMatricesUniform = shadowMappingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjectionMatrix");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow debugging vertex shader...";

    auto shadowDebuggingVertexSource = globjects::Shader::sourceFromFile("media/shadow-debug.vert");
    auto shadowDebuggingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowDebuggingVertexSource.get());
    auto shadowDebuggingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowDebuggingVertexShaderTemplate.get());

    if (!shadowDebuggingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow debugging fragment shader...";

    auto shadowDebuggingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-debug.frag");
    auto shadowDebuggingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowDebuggingFragmentSource.get());
    auto shadowDebuggingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowDebuggingFragmentShaderTemplate.get());

    if (!shadowDebuggingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating shadow debugging program...";

    auto shadowDebuggingProgram = std::make_unique<globjects::Program>();
    shadowDebuggingProgram->attach(shadowDebuggingVertexShader.get(), shadowDebuggingFragmentShader.get());

    auto shadowDebuggingModelTransformationUniform = shadowDebuggingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling primitive rendering vertex shader...";

    auto primitiveRenderingVertexSource = globjects::Shader::sourceFromFile("media/primitive-rendering.vert");
    auto primitiveRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(primitiveRenderingVertexSource.get());
    auto primitiveRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), primitiveRenderingVertexShaderTemplate.get());

    if (!primitiveRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling primitive rendering fragment shader...";

    auto primitiveRenderingFragmentSource = globjects::Shader::sourceFromFile("media/primitive-rendering.frag");
    auto primitiveRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(primitiveRenderingFragmentSource.get());
    auto primitiveRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), primitiveRenderingFragmentShaderTemplate.get());

    if (!primitiveRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating primitive rendering program...";

    auto primitiveRenderingProgram = std::make_unique<globjects::Program>();
    primitiveRenderingProgram->attach(primitiveRenderingVertexShader.get(), primitiveRenderingFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering vertex shader...";

    auto shadowRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.vert");
    auto shadowRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingVertexShaderSource.get());
    auto shadowRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowRenderingVertexShaderTemplate.get());

    if (!shadowRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile shadow rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering fragment shader...";

    auto shadowRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.frag");
    auto shadowRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingFragmentShaderSource.get());
    auto shadowRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowRenderingFragmentShaderTemplate.get());

    if (!shadowRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile chicken fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Creating shadow rendering program...";

    auto shadowRenderingProgram = std::make_unique<globjects::Program>();
    shadowRenderingProgram->attach(shadowRenderingVertexShader.get(), shadowRenderingFragmentShader.get());

    auto shadowRenderingModelTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("model");
    auto shadowRenderingViewTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("view");
    auto shadowRenderingProjectionTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("projection");

    auto shadowRenderingLightPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto shadowRenderingLightColorUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto shadowRenderingCameraPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    auto shadowRenderingLightViewProjectionsUniform = shadowRenderingProgram->getUniform<std::vector<glm::mat4>>("lightViewProjections");
    auto shadowRenderingSplitsUniform = shadowRenderingProgram->getUniform<std::vector<float>>("splits"); // distance from camera to zFar, e.g. (far - cameraPosition.z) * splitFraction

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto chickenScene = importer.ReadFile("media/Chicken.3ds", 0);

    if (!chickenScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto chickenModel = Model::fromAiNode(chickenScene, chickenScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    chickenModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), glm::radians(-90.0f), glm::vec3(1.0f, 0, 0)));

    auto quadScene = importer.ReadFile("media/quad.obj", 0);

    if (!quadScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto quadModel = Model::fromAiNode(quadScene, quadScene->mRootNode);

    quadModel->setTransformation(glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-5, 0, 5)), glm::vec3(10.0f, 0, 10.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0, 0)));

    sf::Image textureImage;

    if (!textureImage.loadFromFile("media/texture.jpg"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    textureImage.flipVertically();

    auto defaultTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    defaultTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    defaultTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    defaultTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(textureImage.getSize().x, textureImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr()));

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing shadowMapTexture...";

    auto shadowMapTexture = std::make_unique<globjects::Texture>(gl::GL_TEXTURE_2D_ARRAY);

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    shadowMapTexture->storage3D(
        4,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT32F),
        glm::vec3(2048, 2048, 4) // this last `4` is the number of layers of a 3D texture; must be equal to the number of frustum splits we are making
    );

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing frame buffer...";

    auto framebuffer = std::make_unique<globjects::Framebuffer>();
    framebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), shadowMapTexture.get());

    framebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;

#ifdef WIN32
    const float cameraRotateSpeed = 10.0f;
#else
    const float cameraRotateSpeed = 20.0f;
#endif

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    glm::mat4 initialCameraProjection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

    glm::mat4 initialCameraView = glm::lookAt(
        cameraPos,
        cameraPos + cameraForward,
        cameraUp);

    /*std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
        {
            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        }
    };*/

    std::array<GLuint, 6 * 2 * 3> frustumIndices {
        {
            0, 1, 2,
            2, 3, 0, // front

            4, 5, 6,
            6, 7, 4, // back

            0, 1, 5,
            5, 4, 0, // bottom

            3, 7, 6,
            6, 2, 3, // top

            0, 4, 7,
            7, 3, 0, // left

            1, 2, 6,
            6, 5, 1, // right
        }
    };

    std::cout << "[DEBUG] Preparing frustum debugging VAO..." << std::endl;

    glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 4.0f); // cameraPos;

    const float nearPlane = 0.1f;
    const float farPlane = 20.0f;

    // these vertices define view frustum in screen space coordinates
    std::array<glm::vec3, 8> _frustumCornerVertices{
        {
            { -1.0f, -1.0f, nearPlane }, { 1.0f, -1.0f, nearPlane }, { 1.0f, 1.0f, nearPlane }, { -1.0f, 1.0f, nearPlane },
            { -1.0f, -1.0f, farPlane }, { 1.0f, -1.0f, farPlane }, { 1.0f, 1.0f, farPlane }, { -1.0f, 1.0f, farPlane },
        }
    };

    std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
        {
            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        }
    };

    auto _frustumVAO = std::make_unique<globjects::VertexArray>();

    auto _frustumIndexBuffer = std::make_unique<globjects::Buffer>();
    auto _frustumVertexBuffer = std::make_unique<globjects::Buffer>();

    _frustumIndexBuffer->setData(frustumIndices, static_cast<gl::GLenum>(GL_STATIC_DRAW));
    _frustumVertexBuffer->setData(_frustumCornerVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

    std::cout << "[DEBUG] Binding element buffer..." << std::endl;

    _frustumVAO->bindElementBuffer(_frustumIndexBuffer.get());

    std::cout << "[DEBUG] Binding vertex buffer..." << std::endl;

    _frustumVAO->binding(0)->setAttribute(0);
    _frustumVAO->binding(0)->setBuffer(_frustumVertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
    _frustumVAO->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
    _frustumVAO->enable(0);

    std::vector<glm::vec4> _splitColors{
        { 0.5f, 0.5f, 0.7f, 0.4f },
        { 0.0f, 1.0f, 0.0f, 0.4f },
        { 0.0f, 0.0f, 1.0f, 0.4f },
        { 1.0f, 1.0f, 0.0f, 0.4f },
        { 1.0f, 0.0f, 0.0f, 0.4f },
        { 0.9f, 0.9f, 0.9f, 0.4f },
    };

    std::cout << "[DEBUG] Done" << std::endl;

    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // project shadows onto cascades, aka frustum splits
    // 0.05x, 0.2x, 0.5x, 1x of the entire view frustum

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool renderLightFrusta = false;
    bool renderLightProjection = false;

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
#ifdef WIN32
        if (!window.hasFocus())
        {
            continue;
        }
#endif

        sf::Event event {};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
            {
                renderLightFrusta = !renderLightFrusta;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
            {
                renderLightProjection = !renderLightProjection;
            }

            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

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
                cameraPos += cameraForward * cameraMoveSpeed * 10.0f * deltaTime;
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
                cameraPos -= cameraForward * cameraMoveSpeed * 10.0f * deltaTime;
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
                cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * 10.0f * deltaTime;
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
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * 10.0f * deltaTime;
            }
            else
            {
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
            }
        }

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        std::vector<glm::mat4> lightViewProjectionMatrices;

        {
            // the code below renders the camera frustum
            auto proj = glm::inverse(cameraProjection * cameraView);

            std::array<glm::vec3, 8> _frustumVertices;

            std::vector<float> splits{ { 0.0f, 0.05f, 0.2f, 0.5f, 1.0f } };

            float _nearPlane = 1.0f;
            float _farPlane = 1.0f; // _nearPlane - (0.02 * (farPlane - nearPlane))

            for (auto i = 1; i < splits.size(); ++i)
            {
                _nearPlane = _farPlane;
                _farPlane = 1.0f - (2.0f * splits[i]);

                std::array<glm::vec3, 8> _cameraFrustumSliceCornerVertices{
                    {
                        { -1.0f, -1.0f, _nearPlane }, { 1.0f, -1.0f, _nearPlane }, { 1.0f, 1.0f, _nearPlane }, { -1.0f, 1.0f, _nearPlane },
                        { -1.0f, -1.0f, _farPlane }, { 1.0f, -1.0f, _farPlane }, { 1.0f, 1.0f, _farPlane }, { -1.0f, 1.0f, _farPlane },
                    }
                };

                std::transform(
                    _cameraFrustumSliceCornerVertices.begin(),
                    _cameraFrustumSliceCornerVertices.end(),
                    _frustumVertices.begin(),
                    [&](glm::vec3 p) {
                        auto v = proj * glm::vec4(p, 1.0f);
                        // auto v1 = glm::vec3(v) / v.w; // v, frustum split' corner vertex, in world space // why divide by `w` component if we could just continue multiplying by matrix
                        auto u = lightView * v; // glm::vec4(v1, 1.0f); // v1 in light view space
                        return glm::vec3(u) / u.w;
                    }
                );

                // at this stage, _frustumVertices are the vertices of camera frustum split, in light view space

                // calculate frustum slice' AABB
                float minX = 0.0f, maxX = 0.0f;
                float minY = 0.0f, maxY = 0.0f;
                float minZ = 0.0f, maxZ = 0.0f;

                for (auto i = 0; i < _frustumVertices.size(); ++i)
                {
                    auto p = _frustumVertices[i];

                    if (i == 0)
                    {
                        minX = maxX = p.x;
                        minY = maxY = p.y;
                        minZ = maxZ = p.z;
                    }
                    else
                    {
                        minX = std::fmin(minX, p.x);
                        minY = std::fmin(minY, p.y);
                        minZ = std::fmin(minZ, p.z);

                        maxX = std::fmax(maxX, p.x);
                        maxY = std::fmax(maxY, p.y);
                        maxZ = std::fmax(maxZ, p.z);
                    }
                }

                // quality optimization: make ortho projection square:
                minX = -maxX; minY = -maxY; minZ = -maxZ;

                auto frustumSplitProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ) * lightView;

                lightViewProjectionMatrices.push_back(frustumSplitProjectionMatrix);
            }

            std::vector<float> splitDepths{ 0.05f * (farPlane - nearPlane), 0.2f * (farPlane - nearPlane), 0.5f * (farPlane - nearPlane), (farPlane - nearPlane) };

            shadowMappingLightViewProjectionMatrices->set(lightViewProjectionMatrices);
            shadowRenderingLightViewProjectionsUniform->set(lightViewProjectionMatrices);
            shadowRenderingSplitsUniform->set(splitDepths);
        }

        ::glViewport(0, 0, 2048, 2048);

        // first render pass - shadow mapping

        framebuffer->bind();

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_DEPTH_BUFFER_BIT);
        framebuffer->clearBuffer(static_cast<gl::GLenum>(GL_DEPTH), 0, glm::vec4(1.0f));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_FRONT);

        shadowMappingProgram->use();

        shadowMappingModelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        // the ground plane will get culled, we don't want that
        glDisable(GL_CULL_FACE);

        shadowMappingModelTransformationUniform->set(quadModel->getTransformation());

        quadModel->bind();
        quadModel->draw();
        quadModel->unbind();

        framebuffer->unbind();

        shadowMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadowRenderingProgram->use();

        shadowRenderingProgram->setUniform("splitColors", _splitColors);

        shadowRenderingLightPositionUniform->set(lightPosition);
        shadowRenderingLightColorUniform->set(glm::vec3(1.0, 1.0, 1.0));
        // ambientColorUniform->set(glm::vec3(1.0f, 1.0f, 1.0f));
        // materialSpecularUniform->set(12.0f);
        shadowRenderingCameraPositionUniform->set(cameraPos);

        shadowRenderingProjectionTransformationUniform->set(cameraProjection);
        shadowRenderingViewTransformationUniform->set(cameraView);
        // shadowRenderingLightSpaceMatrixUniform->set(lightSpaceMatrix);

        // draw chicken

        shadowMapTexture->bindActive(0);

        shadowRenderingProgram->setUniform("shadowMaps", 0);
        shadowRenderingProgram->setUniform("diffuseTexture", 1);

        shadowRenderingModelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        shadowRenderingModelTransformationUniform->set(quadModel->getTransformation());

        defaultTexture->bindActive(1);

        quadModel->bind();
        quadModel->draw();
        quadModel->unbind();

        defaultTexture->unbindActive(1);

        shadowMapTexture->unbindActive(0);

        shadowRenderingProgram->release();

        {
            primitiveRenderingProgram->use();

            // render frusta
            if (renderLightFrusta)
            {
                /*for (auto i = 0; i < lightViewProjectionMatrices.size(); ++i)
                {
                    auto _lightProj = lightViewProjectionMatrices[i];

                    // the code below renders the camera frustum
                    const auto proj = glm::inverse(_lightProj * lightView);

                    std::array<glm::vec3, 8> _frustumVertices;

                    std::transform(
                        _cameraFrustumCornerVertices.begin(),
                        _cameraFrustumCornerVertices.end(),
                        _frustumVertices.begin(),
                        [&](glm::vec3 p) {
                            auto v = proj * glm::vec4(p, 1.0f);
                            return glm::vec3(v) / v.w;
                        }
                    );

                    primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                    primitiveRenderingProgram->setUniform("color", _splitColors[i + 1]);

                    _frustumVertexBuffer->setData(_frustumVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                    _frustumVAO->bind();

                    _frustumVAO->drawElements(
                        static_cast<gl::GLenum>(GL_TRIANGLES),
                        frustumIndices.size(),
                        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                        nullptr);

                    _frustumVAO->unbind();
                }*/

                std::vector<float> splits{ { 0.0f, 0.05f, 0.2f, 0.5f, 1.0f } };

                const float _depth = 2.0f; // 1.0f - (-1.0f)

                auto proj = glm::inverse(initialCameraProjection * initialCameraView);

                std::array<glm::vec3, 8> _cameraFrustumSliceCornerVertices{
                        {
                            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
                            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
                        }
                };

                std::array<glm::vec3, 8> _totalFrustumVertices;

                std::transform(
                    _cameraFrustumSliceCornerVertices.begin(),
                    _cameraFrustumSliceCornerVertices.end(),
                    _totalFrustumVertices.begin(),
                    [&](glm::vec3 p) {
                        auto v = proj * glm::vec4(p, 1.0f);
                        // auto v1 = glm::vec3(v) / v.w; // v, frustum split' corner vertex, in world space // why divide by `w` component if we could just continue multiplying by matrix
                        // auto u = lightView * v; // glm::vec4(v1, 1.0f); // v1 in light view space
                        return glm::vec3(v) / v.w;
                    }
                );

                std::array<glm::vec3, 4> _frustumVectors{
                    {
                        _totalFrustumVertices[4] - _totalFrustumVertices[0],
                        _totalFrustumVertices[5] - _totalFrustumVertices[1],
                        _totalFrustumVertices[6] - _totalFrustumVertices[2],
                        _totalFrustumVertices[7] - _totalFrustumVertices[3],
                    }
                };

                for (auto i = 1; i < splits.size(); ++i)
                {
                    std::array<glm::vec3, 8> _frustumSliceVertices{
                        {
                            _totalFrustumVertices[0] + (_frustumVectors[0] * _depth * splits[i - 1]),
                            _totalFrustumVertices[1] + (_frustumVectors[1] * _depth * splits[i - 1]),
                            _totalFrustumVertices[2] + (_frustumVectors[2] * _depth * splits[i - 1]),
                            _totalFrustumVertices[3] + (_frustumVectors[3] * _depth * splits[i - 1]),

                            _totalFrustumVertices[0] + (_frustumVectors[0] * _depth * splits[i]),
                            _totalFrustumVertices[1] + (_frustumVectors[1] * _depth * splits[i]),
                            _totalFrustumVertices[2] + (_frustumVectors[2] * _depth * splits[i]),
                            _totalFrustumVertices[3] + (_frustumVectors[3] * _depth * splits[i]),
                        }
                    };

                    float minX = 0.0f, maxX = 0.0f;
                    float minY = 0.0f, maxY = 0.0f;
                    float minZ = 0.0f, maxZ = 0.0f;

                    for (auto i = 0; i < _frustumSliceVertices.size(); ++i)
                    {
                        auto p = _frustumSliceVertices[i];

                        if (i == 0)
                        {
                            minX = maxX = p.x;
                            minY = maxY = p.y;
                            minZ = maxZ = p.z;
                        }
                        else
                        {
                            minX = std::fmin(minX, p.x);
                            minY = std::fmin(minY, p.y);
                            minZ = std::fmin(minZ, p.z);

                            maxX = std::fmax(maxX, p.x);
                            maxY = std::fmax(maxY, p.y);
                            maxZ = std::fmax(maxZ, p.z);
                        }
                    }

                    auto _orthoWidth = (maxX - minX) * 0.5f;
                    auto _orthoHeight = (maxX - minX) * 0.5f;
                    auto _orthoDepth = (maxX - minX) * 0.5f;

                    auto _ortho = glm::ortho(-_orthoWidth, _orthoWidth, -_orthoHeight, _orthoHeight, -_orthoDepth, _orthoDepth);

                    std::array<glm::vec3, 8> _aabbVertices;

                    std::transform(
                        _cameraFrustumSliceCornerVertices.begin(),
                        _cameraFrustumSliceCornerVertices.end(),
                        _aabbVertices.begin(),
                        [&](glm::vec3 p) {
                            auto v = _ortho * glm::vec4(p, 1.0f); // this apparently won't work, since points must be in minX..maxX / minY..maxY / minZ..maxZ ranges
                            return glm::vec3(v) / v.w;
                        }
                    );

                    std::array<glm::vec3, 8> _frustumSliceAlignedAABBVertices;

                    std::transform(
                        _aabbVertices.begin(),
                        _aabbVertices.end(),
                        _frustumSliceAlignedAABBVertices.begin(),
                        [&](glm::vec3 p) {
                            auto v = lightView * glm::vec4(p, 1.0f);
                            return glm::vec3(v) / v.w;
                        }
                    );

                    primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                    primitiveRenderingProgram->setUniform("color", _splitColors[i]);

                    // _frustumVertexBuffer->setData(_frustumSliceAlignedAABBVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));
                    _frustumVertexBuffer->setData(_aabbVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                    _frustumVAO->bind();

                    _frustumVAO->drawElements(
                        static_cast<gl::GLenum>(GL_TRIANGLES),
                        frustumIndices.size(),
                        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                        nullptr);

                    _frustumVAO->unbind();
                }
            } else
            {
                std::vector<float> splits{ { 0.0f, 0.05f, 0.2f, 0.5f, 1.0f } };

                const float _depth = 2.0f; // 1.0f - (-1.0f)

                auto proj = glm::inverse(initialCameraProjection * initialCameraView);

                std::array<glm::vec3, 8> _cameraFrustumSliceCornerVertices{
                        {
                            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
                            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
                        }
                };

                std::array<glm::vec3, 8> _totalFrustumVertices;

                std::transform(
                    _cameraFrustumSliceCornerVertices.begin(),
                    _cameraFrustumSliceCornerVertices.end(),
                    _totalFrustumVertices.begin(),
                    [&](glm::vec3 p) {
                        auto v = proj * glm::vec4(p, 1.0f);
                        // auto v1 = glm::vec3(v) / v.w; // v, frustum split' corner vertex, in world space // why divide by `w` component if we could just continue multiplying by matrix
                        // auto u = lightView * v; // glm::vec4(v1, 1.0f); // v1 in light view space
                        return glm::vec3(v) / v.w;
                    }
                );

                std::array<glm::vec3, 4> _frustumVectors{
                    {
                        _totalFrustumVertices[4] - _totalFrustumVertices[0],
                        _totalFrustumVertices[5] - _totalFrustumVertices[1],
                        _totalFrustumVertices[6] - _totalFrustumVertices[2],
                        _totalFrustumVertices[7] - _totalFrustumVertices[3],
                    }
                };

                for (auto i = 1; i < splits.size(); ++i)
                {
                    std::array<glm::vec3, 8> _frustumVertices{
                        {
                            _totalFrustumVertices[0] + (_frustumVectors[0] * _depth * splits[i - 1]),
                            _totalFrustumVertices[1] + (_frustumVectors[1] * _depth * splits[i - 1]),
                            _totalFrustumVertices[2] + (_frustumVectors[2] * _depth * splits[i - 1]),
                            _totalFrustumVertices[3] + (_frustumVectors[3] * _depth * splits[i - 1]),

                            _totalFrustumVertices[0] + (_frustumVectors[0] * _depth * splits[i]),
                            _totalFrustumVertices[1] + (_frustumVectors[1] * _depth * splits[i]),
                            _totalFrustumVertices[2] + (_frustumVectors[2] * _depth * splits[i]),
                            _totalFrustumVertices[3] + (_frustumVectors[3] * _depth * splits[i]),
                        }
                    };

                    primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                    primitiveRenderingProgram->setUniform("color", _splitColors[i]);

                    _frustumVertexBuffer->setData(_frustumVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                    _frustumVAO->bind();

                    _frustumVAO->drawElements(
                        static_cast<gl::GLenum>(GL_TRIANGLES),
                        frustumIndices.size(),
                        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                        nullptr);

                    _frustumVAO->unbind();
                }
            }

            if (renderLightProjection)
            // light frustum
            {
                // the code below renders the camera frustum
                const auto proj = glm::inverse(lightProjection * lightView);

                std::array<glm::vec3, 8> _frustumVertices;

                std::transform(
                    _cameraFrustumCornerVertices.begin(),
                    _cameraFrustumCornerVertices.end(),
                    _frustumVertices.begin(),
                    [&](glm::vec3 p) {
                        auto v = proj * glm::vec4(p, 1.0f);
                        return glm::vec3(v) / v.w;
                    }
                );

                primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                primitiveRenderingProgram->setUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));

                _frustumVertexBuffer->setData(_frustumVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                _frustumVAO->bind();

                _frustumVAO->drawElements(
                    static_cast<gl::GLenum>(GL_TRIANGLES),
                    frustumIndices.size(),
                    static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                    nullptr);

                _frustumVAO->unbind();
            }

            primitiveRenderingProgram->release();
        }

        // render quad with depth (shadow) map
        /*shadowDebuggingProgram->use();

        shadowMapTexture->bindActive(0);

        for (auto i = 0; i < 4; ++i)
        {
            shadowDebuggingModelTransformationUniform->set(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / (4 * (1.0f + 0.1f)))), glm::vec3(-1.0f + (i * (2.0f / (4 * 1.1f))) + (-0.05f * i), -1.0f, 0.f)));
            shadowDebuggingProgram->setUniform("textureLayer", i);

            quadModel->bind();
            quadModel->draw();
            quadModel->unbind();
        }

        shadowMapTexture->unbindActive(0);

        shadowDebuggingProgram->release();*/

        // done rendering the frame

        window.display();
    }

    return 0;
}
