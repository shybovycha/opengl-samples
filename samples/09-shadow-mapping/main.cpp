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

    std::cout << "[DEBUG] Preparing frustum debugging VAO..." << std::endl;

    glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 4.0f); // cameraPos;

    const float nearPlane = 0.1f;
    const float farPlane = 20.0f;

    // these vertices define view frustum in screen space coordinates
    std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
        {
            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        }
    };

    std::array<GLuint, 6 * 2 * 3> frustumIndices{
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

    auto _frustumVAO = std::make_unique<globjects::VertexArray>();

    auto _frustumIndexBuffer = std::make_unique<globjects::Buffer>();
    auto _frustumVertexBuffer = std::make_unique<globjects::Buffer>();

    _frustumIndexBuffer->setData(frustumIndices, static_cast<gl::GLenum>(GL_STATIC_DRAW));
    _frustumVertexBuffer->setData(_cameraFrustumCornerVertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

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

    const glm::vec3 _lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - lightPosition); // TODO: update to some constant

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool renderLightFrusta = false;
    bool renderLightProjection = false;
    bool debugOn = false;
    bool debugShadowMaps = false;
    bool debugProjectionSliceDimensions = false;

    glm::mat4 _originalCameraProjection(1.0f);
    glm::mat4 _originalCameraView(1.0f);
    glm::vec3 _originalCameraPos(0.0f);
    glm::vec3 _originalCameraForward(0.0f);

    bool isDebuggingLight = false;

    glm::mat4 cameraProjection(1.0f);
    glm::mat4 cameraView(1.0f);

    struct FrustumSlice
    {
        std::array<glm::vec3, 8> vertices;
        glm::vec3 center;
        float radius;
        glm::vec4 color;
        glm::mat4 projection;
    };

    std::vector<glm::mat4> lightViewProjectionMatrices;
    std::vector<float> splitDepths;
    std::vector<glm::mat4> lightProjections;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightOrigins;

    int lightDebuggingView = -1;
    int prevLightDebuggingView = -1;

    std::vector<float> splits{ { 0.0f, 0.05f, 0.2f, 0.5f, 1.0f } };

    std::array<glm::vec3, 8> _cameraFrustumSliceCornerVertices{
        {
            { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        }
    };

    std::vector<FrustumSlice> frustumSlices;

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
        sf::Event event {};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    debugProjectionSliceDimensions = true;
                }

                if (event.key.code == sf::Keyboard::Return)
                {
                    renderLightProjection = !renderLightProjection;
                }

                if (event.key.code == sf::Keyboard::Q)
                {
                    initialCameraProjection = cameraProjection;
                    initialCameraView = cameraView;
                    debugOn = !debugOn;
                }

                if (event.key.code == sf::Keyboard::E)
                {
                    debugShadowMaps = !debugShadowMaps;
                }

                if (event.key.code == sf::Keyboard::Num1)
                {
                    lightDebuggingView = 1;
                }

                if (event.key.code == sf::Keyboard::Num2)
                {
                    lightDebuggingView = 2;
                }

                if (event.key.code == sf::Keyboard::Num3)
                {
                    lightDebuggingView = 3;
                }

                if (event.key.code == sf::Keyboard::Num4)
                {
                    lightDebuggingView = 4;
                }

                if (event.key.code == sf::Keyboard::Num0)
                {
                    lightDebuggingView = -1;
                }
            }

            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        if (prevLightDebuggingView != lightDebuggingView)
        {
            if (lightDebuggingView > -1)
            {
                if (!isDebuggingLight)
                {
                    _originalCameraProjection = cameraProjection;
                    _originalCameraView = cameraView;
                    _originalCameraPos = cameraPos;
                    _originalCameraForward = cameraForward;
                    isDebuggingLight = true;
                }

                cameraPos = frustumSlices[lightDebuggingView - 1].center - glm::normalize(_lightDirection) * frustumSlices[lightDebuggingView - 1].radius;
                cameraForward = frustumSlices[lightDebuggingView - 1].center - cameraPos;
                cameraProjection = frustumSlices[lightDebuggingView - 1].projection;
                cameraView = glm::mat4(1.0f);
            }
            else
            {
                cameraProjection = _originalCameraProjection;
                cameraView = _originalCameraView;
                cameraPos = _originalCameraPos;
                cameraForward = _originalCameraForward;
                isDebuggingLight = false;
            }

            prevLightDebuggingView = lightDebuggingView;
        }

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

#ifdef WIN32
        glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));
        sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
#else
        glm::vec2 mouseDelta = currentMousePos - previousMousePos;
        previousMousePos = currentMousePos;
#endif

        // if (!isDebuggingLight)
        {
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

            if (!isDebuggingLight)
            {
                cameraProjection = glm::perspective(glm::radians(fov), static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y), 0.1f, 100.0f);
            }

            cameraView = glm::lookAt(
                cameraPos,
                cameraPos + cameraForward,
                cameraUp);
        }

        {
            lightViewProjectionMatrices.clear();
            splitDepths.clear();
            lightPositions.clear();
            lightOrigins.clear();

            frustumSlices.clear();

            glm::mat4 proj;

            if (debugOn)
            {
                proj = glm::inverse(initialCameraProjection * initialCameraView);
            }
            else
            {
                proj = glm::inverse(cameraProjection * cameraView);
            }

            std::array<glm::vec3, 8> _entireFrustum;

            std::transform(
                _cameraFrustumSliceCornerVertices.begin(),
                _cameraFrustumSliceCornerVertices.end(),
                _entireFrustum.begin(),
                [&](glm::vec3 p) {
                    glm::vec4 v = proj * glm::vec4(p, 1.0f);
                    return glm::vec3(v) / v.w;
                }
            );

            std::array<glm::vec3, 4> _frustumEdgeDirections;

            for (auto i = 0; i < 4; ++i)
            {
                _frustumEdgeDirections[i] = glm::normalize(_entireFrustum[4 + i] - _entireFrustum[i]);
            }

            const float _depth = 100.0f - 0.1f;

            for (auto splitIdx = 1; splitIdx < splits.size(); ++splitIdx)
            {
                // frustum slice vertices in world space
                std::array<glm::vec3, 8> _frustumSliceVertices;

                for (auto t = 0; t < 4; ++t)
                {
                    _frustumSliceVertices[t] = _entireFrustum[t] + _frustumEdgeDirections[t] * _depth * splits[splitIdx - 1];
                    _frustumSliceVertices[4 + t] = _entireFrustum[t] + _frustumEdgeDirections[t] * _depth * splits[splitIdx];
                }

                // TODO: also check if camera is looking towards the light

                glm::vec3 _frustumSliceCenter(0.0f);

                for (auto p : _frustumSliceVertices)
                {
                    _frustumSliceCenter += p;
                }

                _frustumSliceCenter /= 8.0f;

                glm::vec3 _frustumRadiusVector(0.0f);

                for (auto p : _frustumSliceVertices)
                {
                    auto v = p - _frustumSliceCenter;

                    if (glm::length(_frustumRadiusVector) < glm::length(v))
                    {
                        _frustumRadiusVector = v;
                    }
                }

                // calculate new light projection
                glm::vec3 _forward = glm::normalize(_lightDirection);
                glm::vec3 _right = glm::cross(_forward, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 _up(0.0f, 1.0f, 0.0f);
                glm::mat4 _lightView = glm::lookAt(_frustumSliceCenter - glm::normalize(_lightDirection) * glm::length(_frustumRadiusVector), _frustumSliceCenter, _up);

                const float _frustumRadius = glm::length(_frustumRadiusVector);

                glm::mat4 _lightProjectionViewMatrix = glm::ortho(
                    _frustumSliceCenter.x - _frustumRadius,
                    _frustumSliceCenter.x + _frustumRadius,
                    _frustumSliceCenter.y - _frustumRadius,
                    _frustumSliceCenter.y + _frustumRadius,
                    0.0f,
                    _frustumSliceCenter.z + 2.0f * _frustumRadius
                ) * _lightView;

                // push new frustum slice to the vector
                frustumSlices.push_back({ _frustumSliceVertices, _frustumSliceCenter, _frustumRadius, _splitColors[splitIdx], _lightProjectionViewMatrix });

                lightViewProjectionMatrices.push_back(_lightProjectionViewMatrix);

                splitDepths.push_back(_depth * splits[splitIdx] * 0.7f);
            }

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
        shadowRenderingCameraPositionUniform->set(cameraPos);

        shadowRenderingProjectionTransformationUniform->set(cameraProjection);
        shadowRenderingViewTransformationUniform->set(cameraView);

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

        if (debugOn)
        {
            glDisable(GL_CULL_FACE);

            primitiveRenderingProgram->use();

            // render frusta
            auto proj = glm::inverse(initialCameraProjection * initialCameraView);

            std::array<glm::vec3, 8> _entireFrustum;

            std::transform(
                _cameraFrustumSliceCornerVertices.begin(),
                _cameraFrustumSliceCornerVertices.end(),
                _entireFrustum.begin(),
                [&](glm::vec3 p) {
                    glm::vec4 v = proj * glm::vec4(p, 1.0f);
                    return glm::vec3(v) / v.w;
                }
            );

            std::array<glm::vec3, 4> _frustumEdgeDirections;

            for (auto i = 0; i < 4; ++i)
            {
                _frustumEdgeDirections[i] = glm::normalize(_entireFrustum[4 + i] - _entireFrustum[i]);
            }

            const float _depth = 100.0f - 0.1f;

            for (auto splitIdx = 1; splitIdx < splits.size(); ++splitIdx)
            {
                auto slice = frustumSlices[splitIdx - 1];

                // light frustum
                if (renderLightProjection)
                {
                    glm::vec3 _lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - lightPosition); // TODO: update to some constant
                    glm::vec3 _lightPosition = slice.center - glm::normalize(_lightDirection) * slice.radius;

                    std::array<glm::vec3, 2> _lightBeam{
                        _lightPosition, slice.center
                    };

                    primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                    primitiveRenderingProgram->setUniform("color", slice.color);

                    _frustumVertexBuffer->setData(_lightBeam, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                    _frustumVAO->bind();

                    _frustumVAO->drawArrays(
                        static_cast<gl::GLenum>(GL_LINES),
                        0,
                        2
                    );

                    _frustumVAO->unbind();
                }

                primitiveRenderingProgram->setUniform("transformation", cameraProjection * cameraView);
                primitiveRenderingProgram->setUniform("color", slice.color);

                _frustumVertexBuffer->setData(slice.vertices, static_cast<gl::GLenum>(GL_DYNAMIC_DRAW));

                _frustumVAO->bind();

                _frustumVAO->drawElements(
                    static_cast<gl::GLenum>(GL_TRIANGLES),
                    frustumIndices.size(),
                    static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                    nullptr);

                _frustumVAO->unbind();
            }

            if (debugProjectionSliceDimensions)
            {
                debugProjectionSliceDimensions = false;
            }

            primitiveRenderingProgram->release();

            glEnable(GL_CULL_FACE);
        }

        // render quad with depth (shadow) map
        if (debugShadowMaps)
        {
            shadowDebuggingProgram->use();

            shadowMapTexture->bindActive(0);

            const auto imageWidth = (window.getSize().x / 4) - 20;
            const auto imageHeight = imageWidth;

            for (auto i = 0; i < 4; ++i)
            {
                ::glViewport(20 + (i * (imageWidth + 20)), 0, imageWidth, imageHeight);

                shadowDebuggingModelTransformationUniform->set(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 0.f)));
                shadowDebuggingProgram->setUniform("textureLayer", i);

                quadModel->bind();
                quadModel->draw();
                quadModel->unbind();
            }

            shadowMapTexture->unbindActive(0);

            shadowDebuggingProgram->release();
        }

        // done rendering the frame

        window.display();
    }

    return 0;
}
