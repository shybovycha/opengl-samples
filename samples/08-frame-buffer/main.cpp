#include <iostream>
#include <sstream>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
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

    static std::unique_ptr<Mesh> fromAiMesh(const aiScene* scene, aiMesh* mesh)
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

                std::cout << "[INFO] Loading DIFFUSE texture " << str.C_Str() << "...";

                sf::Image textureImage;

                if (!textureImage.loadFromFile(str.C_Str()))
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

    static std::unique_ptr<Model> fromAiNode(const aiScene* scene, aiNode* node)
    {
        std::vector<std::unique_ptr<Mesh>> meshes;

        processAiNode(scene, node, meshes);

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
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::unique_ptr<Mesh>>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = Mesh::fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]]);
            meshes.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processAiNode(scene, child, meshes);
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

    sf::Window window(videoMode, "Hello, Frame buffer!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling vertex shader...";

    auto vertexProgram = std::make_unique<globjects::Program>();
    auto vertexShaderSource = globjects::Shader::sourceFromFile("media/vertex.glsl");
    auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexShaderSource.get());
    auto vertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

    if (!vertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    vertexProgram->attach(vertexShader.get());

    auto modelTransformationUniform = vertexProgram->getUniform<glm::mat4>("model");
    auto viewTransformationUniform = vertexProgram->getUniform<glm::mat4>("view");
    auto projectionTransformationUniform = vertexProgram->getUniform<glm::mat4>("projection");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling fragment shader...";

    auto fragmentProgram = std::make_unique<globjects::Program>();
    auto fragmentShaderSource = globjects::Shader::sourceFromFile("media/fragment.glsl");
    auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentShaderSource.get());
    auto fragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

    if (!fragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    fragmentProgram->attach(fragmentShader.get());

    auto lightPositionUniform = fragmentProgram->getUniform<glm::vec3>("lightPosition");
    auto lightColorUniform = fragmentProgram->getUniform<glm::vec3>("lightColor");
    auto ambientColorUniform = fragmentProgram->getUniform<glm::vec3>("ambientColor");
    auto materialSpecularUniform = fragmentProgram->getUniform<float>("materialSpecular");
    auto cameraPositionUniform = fragmentProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;

    /*std::cout << "[INFO] Compiling depth fragment shader...";

    auto depthFragmentProgram = std::make_unique<globjects::Program>();
    auto depthFragmentShaderSource = globjects::Shader::sourceFromFile("media/depth-fragment.glsl");
    auto depthFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(depthFragmentShaderSource.get());
    auto depthFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), depthFragmentShaderTemplate.get());

    if (!depthFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile depth fragment shader" << std::endl;
        return 1;
    }

    depthFragmentProgram->attach(depthFragmentShader.get());

    auto depthCameraPositionUniform = depthFragmentProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;*/

    std::cout << "[INFO] Creating rendering pipeline...";

    auto programPipeline = std::make_unique<globjects::ProgramPipeline>();

    programPipeline->useStages(vertexProgram.get(), gl::GL_VERTEX_SHADER_BIT);
    programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto scene = importer.ReadFile("media/Chicken.3ds",
        aiProcess_Triangulate
        // | aiProcess_CalcTangentSpace
        // | aiProcess_JoinIdenticalVertices
        // | aiProcess_SortByPType
        | aiProcess_RemoveRedundantMaterials
        | aiProcess_GenUVCoords
        | aiProcess_GenNormals
        | aiProcess_TransformUVCoords
    );

    if (!scene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto model = Model::fromAiNode(scene, scene->mRootNode);

    auto scene2 = importer.ReadFile("media/cube.obj",
        aiProcess_Triangulate
        // | aiProcess_CalcTangentSpace
        // | aiProcess_JoinIdenticalVertices
        // | aiProcess_SortByPType
        | aiProcess_RemoveRedundantMaterials
        | aiProcess_GenUVCoords
        | aiProcess_GenNormals
        | aiProcess_TransformUVCoords
    );

    if (!scene2)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto cubeModel = Model::fromAiNode(scene2, scene2->mRootNode);

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    auto transformation = glm::mat4(1.0f);

    transformation = glm::scale(transformation, glm::vec3(0.01f));
    transformation = glm::rotate(transformation, glm::radians(-90.0f), glm::vec3(1.0f, 0, 0));

    model->setTransformation(transformation);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    /*auto depthTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    depthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    depthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    depthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    depthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    std::cout << "[depthTexture";

    depthTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    std::cout << " - ok]";*/

    /*auto depthColorTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    depthColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    std::cout << "[depthColorTexture";

    depthColorTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB8),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_UNSIGNED_INT),
        nullptr
    );

    std::cout << " - ok]";

    auto depthFramebuffer = std::make_unique<globjects::Framebuffer>();

    std::cout << "[color]";
    depthFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), depthColorTexture.get());
    // std::cout << "[depth]";
    // depthFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), depthTexture.get());

    depthFramebuffer->setDrawBuffer(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0));*/

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);

    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.getSize().x, window.getSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.getSize().x, window.getSize().y);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[ERROR] Framebuffer incomplete" << std::endl;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

#ifdef WIN32
        glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));
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

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 100.0f);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        modelTransformationUniform->set(model->getTransformation());
        viewTransformationUniform->set(view);
        projectionTransformationUniform->set(projection);

        lightPositionUniform->set(glm::vec3(-2, 2, 2));
        lightColorUniform->set(glm::vec3(1, 0.5, 0.5));
        ambientColorUniform->set(glm::vec3(1.0f, 1.0f, 1.0f));
        materialSpecularUniform->set(12.0f);
        cameraPositionUniform->set(cameraPos);

        // depthCameraPositionUniform->set(cameraPos);

        // first render pass - render depth to texture

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // depthFramebuffer->bind();

        // depthFramebuffer->clearBuffer(static_cast<gl::GLenum>(GL_COLOR), 0, glm::vec4(0.0));
        // depthFramebuffer->clearBuffer(static_cast<gl::GLenum>(GL_DEPTH), 0, glm::vec4(0.0));

        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LESS);

        programPipeline->use();

        // programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

        model->bind();
        model->draw();
        model->unbind();

        // depthFramebuffer->unbind();

        /*depthFramebuffer->blit(
            static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
            std::array<gl::GLint, 4> { { 0, 0, static_cast<gl::GLint>(window.getSize().x), static_cast<gl::GLint>(window.getSize().y) } },
            globjects::Framebuffer::defaultFBO().get(),
            static_cast<gl::GLenum>(GL_BACK_LEFT),
            std::array<gl::GLint, 4> { { 0, 0, static_cast<gl::GLint>(window.getSize().x), static_cast<gl::GLint>(window.getSize().y) } },
            static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT),
            static_cast<gl::GLenum>(GL_NEAREST)
        );*/

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        cubeModel->bind();

        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

        modelTransformationUniform->set(glm::mat4(1.0f));

        // programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

        // depthColorTexture->bind();
        cubeModel->draw();
        // depthColorTexture->unbind();
        cubeModel->unbind();

        // glDisable(GL_DEPTH_TEST);

        // second pass - switch to normal shader and render picture with depth information to the viewport

        /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

        model->bind();
        depthColorTexture->bindActive(1);
        model->draw();
        depthColorTexture->unbindActive(1);
        model->unbind();*/

        programPipeline->release();

        window.display();

#ifdef WIN32
        sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
#endif
    }

    return 0;
}
