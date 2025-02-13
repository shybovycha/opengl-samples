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

                    const auto& filePath = std::filesystem::current_path() / path / imagePath;

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
            texture->bind();
        }
    }

    void unbind()
    {
        for (auto& texture : m_textures)
        {
            texture->unbind();
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

    sf::Window window(videoMode, "Hello, Model loading!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling model rendering vertex shader...";

    auto modelRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/model-rendering.vert");
    auto modelRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(modelRenderingVertexShaderSource.get());
    auto modelRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), modelRenderingVertexShaderTemplate.get());

    if (!modelRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile model rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling model rendering fragment shader...";

    auto modelRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/model-rendering.frag");
    auto modelRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(modelRenderingFragmentShaderSource.get());
    auto modelRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), modelRenderingFragmentShaderTemplate.get());

    if (!modelRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile chicken fragment shader" << std::endl;
        return 1;
    }

    auto modelRenderingProgram = std::make_unique<globjects::Program>();
    modelRenderingProgram->attach(modelRenderingVertexShader.get(), modelRenderingFragmentShader.get());

    auto modelTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("model");
    auto viewTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("view");
    auto projectionTransformationUniform = modelRenderingProgram->getUniform<glm::mat4>("projection");

    auto lightPositionUniform = modelRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto lightColorUniform = modelRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto ambientColorUniform = modelRenderingProgram->getUniform<glm::vec3>("ambientColor");
    auto diffuseColorUniform = modelRenderingProgram->getUniform<glm::vec3>("diffuseColor");
    auto materialSpecularUniform = modelRenderingProgram->getUniform<float>("materialSpecular");
    auto cameraPositionUniform = modelRenderingProgram->getUniform<glm::vec3>("cameraPosition");

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

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
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
            cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
        }

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        glm::vec3 lightPosition = cameraPos; // cameraPos + (cameraUp * 2.0f);

        viewTransformationUniform->set(view);
        projectionTransformationUniform->set(projection);

        lightPositionUniform->set(lightPosition);
        lightColorUniform->set(glm::vec3(1, 0.5, 0.5));
        ambientColorUniform->set(glm::vec3(1.0f, 1.0f, 1.0f));
        materialSpecularUniform->set(12.0f);
        cameraPositionUniform->set(cameraPos);

        diffuseColorUniform->set(glm::vec4(1.0, 1.0, 1.0, 1.0));

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelRenderingProgram->use();

        // draw chicken

        modelTransformationUniform->set(chickenModel->getTransformation());

        chickenModel->bind();
        chickenModel->draw();
        chickenModel->unbind();

        modelRenderingProgram->release();

        window.display();
    }

    return 0;
}
