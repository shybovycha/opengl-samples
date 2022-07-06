#include "common/stdafx.hpp"

#include "common/AbstractDrawable.hpp"

/*

TODO:

1. replace multiple vectors of data + multiple buffer objects for mesh rendering with one unified buffer and a Vertex struct with multiple params (normal, tangent, uv, position, etc.)
2. StaticMesh would use StaticVertex (no boneIds and boneWeights attributes) whereas AnimatedMesh would use AnimatedVertex (with boneIds and boneWeights attributes)

*/

struct BoneTransformation
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

struct Bone
{
    std::string name;
    std::vector<std::shared_ptr<Bone>> children;
    glm::mat4 transformation;
};

struct AnimationKeyframe
{
    std::vector<BoneTransformation> transformations;
    float timestamp;
};

struct Animation
{
    std::vector<AnimationKeyframe> keyframes;
};

struct ModelAnimator
{
    std::shared_ptr<Animation> animation;
    float time;
};

struct AnimatedVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    // glm::vec3 tangent;
    // glm::vec3 bitangent;
    glm::vec2 uv;
    glm::vec4 boneIds;
    glm::vec4 boneWeights;
};

class AnimatedMesh : public AbstractDrawable
{
    friend class AnimatedMeshBuilder;

public:
    static std::shared_ptr<AnimatedMeshBuilder> builder()
    {
        return std::make_shared<AnimatedMeshBuilder>();
    }

    AnimatedMesh(
        std::vector<AnimatedVertex> vertexData,
        std::vector<unsigned int> indices,
        std::vector<globjects::Texture*> textures,
        std::vector<std::shared_ptr<Animation>> animations,
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> vertexDataBuffer,
        std::unique_ptr<globjects::Buffer> indexBuffer) :

        m_vertexData(std::move(vertexData)),
        m_indices(std::move(indices)),
        m_textures(std::move(textures)),
        m_animations(std::move(animations)),
        m_vao(std::move(vao)),
        m_vertexDataBuffer(std::move(vertexDataBuffer)),
        m_indexBuffer(std::move(indexBuffer))
    {
    }

    void draw() override
    {
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
        m_vao->unbind();

        for (auto& texture : m_textures)
        {
            texture->unbindActive(1);
        }
    }

private:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexDataBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    std::vector<globjects::Texture*> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<AnimatedVertex> m_vertexData;

    std::vector<std::shared_ptr<Animation>> m_animations;
    std::vector<std::unique_ptr<ModelAnimator>> m_animators;
};

class AnimatedMeshBuilder
{
public:
    AnimatedMeshBuilder() :
        m_boneIdsAttributeIndex(0),
        m_boneWeightsAttributeIndex(0),
        m_positionAttributeIndex(0),
        m_normalAttributeIndex(0),
        m_uvAttributeIndex(0)
    {
    }

    AnimatedMeshBuilder* addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
    {
        AnimatedVertex vertex { .position = position, .normal = normal, .uv = uv, .boneIds = glm::u32vec4(), .boneWeights = glm::vec4() };

        m_vertexData.push_back(vertex);
        
        return this;
    }

    AnimatedMeshBuilder* addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec4 boneIds, glm::vec4 boneWeights)
    {
        AnimatedVertex vertex { .position = position, .normal = normal, .uv = uv, .boneIds = boneIds, .boneWeights = boneWeights };

        m_vertexData.push_back(vertex);

        return this;
    }

    AnimatedMeshBuilder* addIndices(std::vector<unsigned int> indices)
    {
        m_indices.insert(indices.end(), indices.begin(), indices.end());

        return this;
    }

    AnimatedMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture)
    {
        m_textures.push_back(texture.get());

        return this;
    }

    AnimatedMeshBuilder* addTexture(globjects::Texture* texture)
    {
        m_textures.push_back(texture);

        return this;
    }

    AnimatedMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        for (auto& texture : textures)
        {
            m_textures.push_back(texture.get());
        }

        return this;
    }

    AnimatedMeshBuilder* addTextures(std::vector<globjects::Texture*> textures)
    {
        m_textures.insert(m_textures.end(), textures.begin(), textures.end());

        return this;
    }

    AnimatedMeshBuilder* addAnimation(std::shared_ptr<Animation> animation)
    {
        m_animations.push_back(animation);

        return this;
    }

    AnimatedMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex)
    {
        m_positionAttributeIndex = positionAttributeIndex;

        return this;
    }

    AnimatedMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex)
    {
        m_normalAttributeIndex = normalAttributeIndex;

        return this;
    }

    AnimatedMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex)
    {
        m_uvAttributeIndex = uvAttributeIndex;

        return this;
    }

    AnimatedMeshBuilder* setBoneIdsAttributerIndex(unsigned int boneIdsAttributeIndex)
    {
        m_boneIdsAttributeIndex = boneIdsAttributeIndex;

        return this;
    }

    AnimatedMeshBuilder* setBoneWeightsAttributerIndex(unsigned int boneWeightsAttributeIndex)
    {
        m_boneWeightsAttributeIndex = boneWeightsAttributeIndex;

        return this;
    }

    std::unique_ptr<AnimatedMesh> build()
    {
        m_vertexDataBuffer = std::make_unique<globjects::Buffer>();

        m_vertexDataBuffer->setData(m_vertexData, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_indexBuffer = std::make_unique<globjects::Buffer>();

        m_indexBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao = std::make_unique<globjects::VertexArray>();

        m_vao->bindElementBuffer(m_indexBuffer.get());

        m_vao->binding(m_positionAttributeIndex)->setAttribute(m_positionAttributeIndex);
        m_vao->binding(m_positionAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(AnimatedVertex, position), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_positionAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_positionAttributeIndex);

        m_vao->binding(m_normalAttributeIndex)->setAttribute(m_normalAttributeIndex);
        m_vao->binding(m_normalAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(AnimatedVertex, normal), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_normalAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_normalAttributeIndex);

        m_vao->binding(m_uvAttributeIndex)->setAttribute(m_uvAttributeIndex);
        m_vao->binding(m_uvAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(AnimatedVertex, uv), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_uvAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_uvAttributeIndex);

        m_vao->binding(m_boneIdsAttributeIndex)->setAttribute(m_boneIdsAttributeIndex);
        m_vao->binding(m_boneIdsAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(AnimatedVertex, boneIds), sizeof(glm::vec4)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_boneIdsAttributeIndex)->setFormat(4, static_cast<gl::GLenum>(GL_UNSIGNED_INT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_boneIdsAttributeIndex);

        m_vao->binding(m_boneWeightsAttributeIndex)->setAttribute(m_boneWeightsAttributeIndex);
        m_vao->binding(m_boneWeightsAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(AnimatedVertex, boneWeights), sizeof(glm::vec4)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_boneWeightsAttributeIndex)->setFormat(4, static_cast<gl::GLenum>(GL_UNSIGNED_INT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_boneWeightsAttributeIndex);

        return std::make_unique<AnimatedMesh>(
            std::move(m_vertexData), 
            std::move(m_indices), 
            std::move(m_textures), 
            std::move(m_animations), 
            std::move(m_vao),
            std::move(m_vertexDataBuffer),
            std::move(m_indexBuffer));
    }

private:
    std::vector<AnimatedVertex> m_vertexData;
    std::vector<unsigned int> m_indices;
    std::vector<globjects::Texture*> m_textures;
    std::vector<std::shared_ptr<Animation>> m_animations;

    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_vertexDataBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    unsigned int m_positionAttributeIndex;
    unsigned int m_normalAttributeIndex;
    unsigned int m_uvAttributeIndex;
    unsigned int m_boneIdsAttributeIndex;
    unsigned int m_boneWeightsAttributeIndex;
};

class AnimatedModel : public AbstractDrawable
{
public:
    AnimatedModel(std::vector<std::unique_ptr<Bone>> skeleton, std::vector<std::unique_ptr<AnimatedMesh>> meshes) :
        AbstractDrawable(),

        m_skeleton(std::move(skeleton)),
        m_skin(std::move(meshes))
    {
    }

    void draw() override
    {
        for (auto& mesh : m_skin)
        {
            mesh->draw();
        }
    }

    void drawInstanced(unsigned int instances) override
    {
        for (auto& mesh : m_skin)
        {
            mesh->drawInstanced(instances);
        }
    }

    void bind() override
    {
        for (auto& mesh : m_skin)
        {
            mesh->bind();
        }
    }

    void unbind() override
    {
        for (auto& mesh : m_skin)
        {
            mesh->unbind();
        }
    }


    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

private:
    std::vector<std::unique_ptr<Bone>> m_skeleton;
    std::vector<std::unique_ptr<AnimatedMesh>> m_skin;
    glm::mat4 m_transformation; // TODO: replace with instances
};

struct StaticVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    // glm::vec3 tangent;
    // glm::vec3 bitangent;
    glm::vec2 uv;
};

class StaticMesh : public AbstractDrawable
{
    friend class StaticMeshBuilder;

public:
    static std::shared_ptr<StaticMeshBuilder> builder()
    {
        return std::make_shared<StaticMeshBuilder>();
    }

    StaticMesh(
        std::vector<StaticVertex> vertexData,
        std::vector<unsigned int> indices,
        std::vector<globjects::Texture*> textures,
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> vertexDataBuffer,
        std::unique_ptr<globjects::Buffer> indexBuffer) :

        m_vertexData(std::move(vertexData)),
        m_indices(std::move(indices)),
        m_textures(textures),
        m_vao(std::move(vao)),
        m_vertexDataBuffer(std::move(vertexDataBuffer)),
        m_indexBuffer(std::move(indexBuffer))
    {
    }

    void draw() override
    {
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
        m_vao->unbind();

        for (auto& texture : m_textures)
        {
            texture->unbindActive(1);
        }
    }

private:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexDataBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    std::vector<globjects::Texture*> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<StaticVertex> m_vertexData;
};

class StaticMeshBuilder
{
public:
    StaticMeshBuilder() :
        m_positionAttributeIndex(0),
        m_normalAttributeIndex(0),
        m_uvAttributeIndex(0)
    {
    }

    StaticMeshBuilder* addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
    {
        StaticVertex vertex { .position = position, .normal = normal, .uv = uv };

        m_vertexData.push_back(vertex);

        return this;
    }

    StaticMeshBuilder* addVertices(std::vector<StaticVertex> vertices)
    {
        m_vertexData.insert(m_vertexData.end(), vertices.begin(), vertices.end());

        return this;
    }

    StaticMeshBuilder* addIndices(std::vector<unsigned int> indices)
    {
        m_indices.insert(indices.end(), indices.begin(), indices.end());

        return this;
    }

    StaticMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture)
    {
        m_textures.push_back(texture.get());

        return this;
    }

    StaticMeshBuilder* addTexture(globjects::Texture* texture)
    {
        m_textures.push_back(texture);

        return this;
    }

    StaticMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        for (auto& texture : textures)
        {
            m_textures.push_back(texture.get());
        }

        return this;
    }

    StaticMeshBuilder* addTextures(std::vector<globjects::Texture*> textures)
    {
        m_textures.insert(m_textures.end(), textures.begin(), textures.end());

        return this;
    }

    StaticMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex)
    {
        m_positionAttributeIndex = positionAttributeIndex;

        return this;
    }

    StaticMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex)
    {
        m_normalAttributeIndex = normalAttributeIndex;

        return this;
    }

    StaticMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex)
    {
        m_uvAttributeIndex = uvAttributeIndex;

        return this;
    }

    std::unique_ptr<StaticMesh> build()
    {
        m_vertexDataBuffer = std::make_unique<globjects::Buffer>();

        m_vertexDataBuffer->setData(m_vertexData, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_indexBuffer = std::make_unique<globjects::Buffer>();

        m_indexBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao = std::make_unique<globjects::VertexArray>();

        m_vao->bindElementBuffer(m_indexBuffer.get());

        m_vao->binding(m_positionAttributeIndex)->setAttribute(m_positionAttributeIndex);
        m_vao->binding(m_positionAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(StaticVertex, position), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_positionAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_positionAttributeIndex);

        m_vao->binding(m_normalAttributeIndex)->setAttribute(m_normalAttributeIndex);
        m_vao->binding(m_normalAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(StaticVertex, normal), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_normalAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_normalAttributeIndex);

        m_vao->binding(m_uvAttributeIndex)->setAttribute(m_uvAttributeIndex);
        m_vao->binding(m_uvAttributeIndex)->setBuffer(m_vertexDataBuffer.get(), offsetof(StaticVertex, uv), sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(m_uvAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(m_uvAttributeIndex);

        return std::make_unique<StaticMesh>(
            std::move(m_vertexData),
            std::move(m_indices),
            std::move(m_textures),
            std::move(m_vao),
            std::move(m_vertexDataBuffer),
            std::move(m_indexBuffer));
    }

private:
    std::vector<StaticVertex> m_vertexData;
    std::vector<unsigned int> m_indices;
    std::vector<globjects::Texture*> m_textures;

    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_vertexDataBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    unsigned int m_positionAttributeIndex;
    unsigned int m_normalAttributeIndex;
    unsigned int m_uvAttributeIndex;
};

class StaticModel : public AbstractDrawable
{
public:
    StaticModel(std::vector<std::unique_ptr<StaticMesh>> meshes) :
        AbstractDrawable(),

        m_meshes(std::move(meshes))
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
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

private:
    std::vector<std::unique_ptr<StaticMesh>> m_meshes;
    glm::mat4 m_transformation; // TODO: replace this with instances
};

class AssimpModelLoader
{
public:
    AssimpModelLoader()
    {
    }

    static std::shared_ptr<StaticModel> staticModelFromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = 0)
    {
        static auto importer = std::make_unique<Assimp::Importer>();
        auto scene = importer->ReadFile(filename, assimpImportFlags);

        if (!scene)
        {
            std::cerr << "failed: " << importer->GetErrorString() << std::endl;
            return nullptr;
        }

        return staticModelFromAiNode(scene, scene->mRootNode, materialLookupPaths);
    }

    static std::shared_ptr<AnimatedModel> animatedModelFromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = 0)
    {
        static auto importer = std::make_unique<Assimp::Importer>();
        auto scene = importer->ReadFile(filename, assimpImportFlags);

        if (!scene)
        {
            std::cerr << "failed: " << importer->GetErrorString() << std::endl;
            return nullptr;
        }

        return animatedModelFromAiNode(scene, scene->mRootNode, materialLookupPaths);
    }

protected:
    static std::shared_ptr<StaticModel> staticModelFromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<std::unique_ptr<StaticMesh>> meshes;

        processStaticAiNode(scene, node, materialLookupPaths, meshes);

        return std::make_shared<StaticModel>(std::move(meshes));
    }

    static std::shared_ptr<AnimatedModel> animatedModelFromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<std::unique_ptr<AnimatedMesh>> skin;
        std::vector<std::unique_ptr<Bone>> skeleton;

        processAnimatedAiNode(scene, node, materialLookupPaths, skin, skeleton);

        return std::make_shared<AnimatedModel>(std::move(skeleton), std::move(skin));
    }

    static void processStaticAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<StaticMesh>>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = staticMeshFromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
            meshes.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processStaticAiNode(scene, child, materialLookupPaths, meshes);
        }
    }

    static void processAnimatedAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<AnimatedMesh>>& skin, std::vector<std::unique_ptr<Bone>>& skeleton)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = animatedMeshFromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
            skin.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processAnimatedAiNode(scene, child, materialLookupPaths, skin, skeleton);
        }
    }

    static std::unique_ptr<StaticMesh> staticMeshFromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<glm::vec2> uvs;

        std::vector<GLuint> indices;

        std::vector<globjects::Texture*> textures;

        loadMeshData(
            scene,
            mesh,
            materialLookupPaths,
            vertices,
            normals,
            tangents,
            bitangents,
            uvs,
            indices,
            textures
        );

        std::vector<StaticVertex> staticVertices;

        for (auto i = 0; i < vertices.size(); ++i)
        {
            StaticVertex vertex { .position = vertices[i], .normal = normals[i], .uv = uvs[i] };

            staticVertices.push_back(vertex);
        }

        return StaticMesh::builder()
            ->addVertices(staticVertices)
            ->addIndices(indices)
            ->addTextures(textures)
            ->build();
    }

    static std::unique_ptr<AnimatedMesh> animatedMeshFromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<glm::vec2> uvs;

        std::vector<GLuint> indices;

        std::vector<globjects::Texture*> textures;

        loadMeshData(
            scene,
            mesh,
            materialLookupPaths,
            vertices,
            normals,
            tangents,
            bitangents,
            uvs,
            indices,
            textures);

        auto builder = AnimatedMesh::builder();

        for (auto i = 0; i < vertices.size(); ++i)
        {
            AnimatedVertex vertex { .position = vertices[i], .normal = normals[i], .uv = uvs[i] };

            builder->addVertex(vertices[i], normals[i], uvs[i]); //, boneIds[i], boneWeights[i]);
        }

        return builder
            ->addIndices(indices)
            ->addTextures(textures)
            ->build();
    }

    /*static void skeletonDataFromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {}, std::shared_ptr<Bone> skeleton)
    {
        if (!mesh->HasBones())
        {
            return;
        }

        mesh->mBones
    }*/

    static void loadMeshData(
        const aiScene* scene, 
        aiMesh* mesh, 
        std::vector<std::filesystem::path>& materialLookupPaths,
        std::vector<glm::vec3>& vertices,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec3>& tangents,
        std::vector<glm::vec3>& bitangents,
        std::vector<glm::vec2>& uvs,
        std::vector<GLuint>& indices,
        std::vector<globjects::Texture*>& textures)
    {
        std::cout << "[INFO] Creating buffer objects...";

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
            const auto face = mesh->mFaces[i];

            for (auto t = 0; t < face.mNumIndices; ++t)
            {
                indices.push_back(face.mIndices[t]);
            }
        }

        std::cout << "done" << std::endl;

        std::cout << "[INFO] Loading textures...";

        if (mesh->mMaterialIndex >= 0)
        {
            const auto material = scene->mMaterials[mesh->mMaterialIndex];

            const auto numDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

            for (auto i = 0; i < numDiffuseTextures; ++i)
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
    }
};


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

    sf::Window window(videoMode, "Hello, Animated model!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling deferred rendering pre-pass vertex shader...";

    auto deferredRenderingPrePassVertexSource = globjects::Shader::sourceFromFile("media/deferred-rendering-pre-pass.vert");
    auto deferredRenderingPrePassVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingPrePassVertexSource.get());
    auto deferredRenderingPrePassVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), deferredRenderingPrePassVertexShaderTemplate.get());

    if (!deferredRenderingPrePassVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering pre-pass vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering pre-pass fragment shader...";

    auto deferredRenderingPrePassFragmentSource = globjects::Shader::sourceFromFile("media/deferred-rendering-pre-pass.frag");
    auto deferredRenderingPrePassFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingPrePassFragmentSource.get());
    auto deferredRenderingPrePassFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), deferredRenderingPrePassFragmentShaderTemplate.get());

    if (!deferredRenderingPrePassFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering pre-pass fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking deferred rendering pre-pass shaders..." << std::endl;

    auto deferredRenderingPrePassProgram = std::make_unique<globjects::Program>();
    deferredRenderingPrePassProgram->attach(deferredRenderingPrePassVertexShader.get(), deferredRenderingPrePassFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering final pass vertex shader...";

    auto deferredRenderingFinalPassVertexSource = globjects::Shader::sourceFromFile("media/deferred-rendering-final-pass.vert");
    auto deferredRenderingFinalPassVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingFinalPassVertexSource.get());
    auto deferredRenderingFinalPassVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), deferredRenderingFinalPassVertexShaderTemplate.get());

    if (!deferredRenderingFinalPassVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering final pass vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling deferred rendering final pass fragment shader...";

    auto deferredRenderingFinalPassFragmentSource = globjects::Shader::sourceFromFile("media/deferred-rendering-final-pass.frag");
    auto deferredRenderingFinalPassFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(deferredRenderingFinalPassFragmentSource.get());
    auto deferredRenderingFinalPassFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), deferredRenderingFinalPassFragmentShaderTemplate.get());

    if (!deferredRenderingFinalPassFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile deferred rendering final pass fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking deferred rendering final pass shaders..." << std::endl;

    auto deferredRenderingFinalPassProgram = std::make_unique<globjects::Program>();
    deferredRenderingFinalPassProgram->attach(deferredRenderingFinalPassVertexShader.get(), deferredRenderingFinalPassFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping vertex shader...";

    auto shadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping-directional.vert");
    auto shadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingVertexSource.get());
    auto shadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowMappingVertexShaderTemplate.get());

    if (!shadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping fragment shader...";

    auto shadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping-directional.frag");
    auto shadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingFragmentSource.get());
    auto shadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowMappingFragmentShaderTemplate.get());

    if (!shadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    auto shadowMappingProgram = std::make_unique<globjects::Program>();
    shadowMappingProgram->attach(shadowMappingVertexShader.get(), shadowMappingFragmentShader.get());

    auto shadowMappingLightSpaceUniform = shadowMappingProgram->getUniform<glm::mat4>("lightSpaceMatrix");
    auto shadowMappingModelTransformationUniform = shadowMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    /* std::cout << "[INFO] Compiling skybox rendering vertex shader...";

    auto skyboxRenderingVertexSource = globjects::Shader::sourceFromFile("media/skybox.vert");
    auto skyboxRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingVertexSource.get());
    auto skyboxRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), skyboxRenderingVertexShaderTemplate.get());

    if (!skyboxRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile skybox rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling skybox rendering fragment shader...";

    auto skyboxRenderingFragmentSource = globjects::Shader::sourceFromFile("media/skybox.frag");
    auto skyboxRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(skyboxRenderingFragmentSource.get());
    auto skyboxRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), skyboxRenderingFragmentShaderTemplate.get());

    if (!skyboxRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile skybox rendering fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking skybox rendering shaders..." << std::endl;

    auto skyboxRenderingProgram = std::make_unique<globjects::Program>();
    skyboxRenderingProgram->attach(skyboxRenderingVertexShader.get(), skyboxRenderingFragmentShader.get());

    auto skyboxRenderingModelTransformationUniform = skyboxRenderingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;*/

    std::cout << "[INFO] Compiling simple vertex shader...";

    auto simpleVertexSource = globjects::Shader::sourceFromFile("media/simple-rendering.vert");
    auto simpleVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleVertexSource.get());
    auto simpleVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), simpleVertexShaderTemplate.get());

    if (!simpleVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling simple fragment shader...";

    auto simpleFragmentSource = globjects::Shader::sourceFromFile("media/simple-rendering.frag");
    auto simpleFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(simpleFragmentSource.get());
    auto simpleFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), simpleFragmentShaderTemplate.get());

    if (!simpleFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile simple fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking simple shaders..." << std::endl;

    auto simpleProgram = std::make_unique<globjects::Program>();
    simpleProgram->attach(simpleVertexShader.get(), simpleFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    auto quadModel = AssimpModelLoader::staticModelFromFile("media/quad.obj", {}, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    auto houseModel = AssimpModelLoader::staticModelFromFile("media/house1.obj", { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    houseModel->setTransformation(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), glm::vec3(0.0f, 0.75f, 0.0f)));

    auto tableModel = AssimpModelLoader::staticModelFromFile("media/table.obj", { "media" });

    tableModel->setTransformation(
        // glm::translate(glm::vec3(0.0f, 0.06f, 0.0f)) *
        glm::scale(glm::vec3(1.0f)) *
        (glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)))
    );

    std::shared_ptr<StaticModel> lanternModel = nullptr;

    {
        lanternModel = AssimpModelLoader::staticModelFromFile("media/lantern.obj", { "media" });

        lanternModel->setTransformation(
            glm::translate(glm::vec3(-1.75f, 3.91f, -0.75f)) *
            glm::scale(glm::vec3(0.5f))
        );

        // TODO: extract this to material class
        auto lanternEmissionMapImage = std::make_unique<sf::Image>();

        if (!lanternEmissionMapImage->loadFromFile("media/lantern_emission.png"))
        {
            std::cerr << "[ERROR] Can not load texture" << std::endl;
            return 1;
        }

        lanternEmissionMapImage->flipVertically();

        auto lanternEmissionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

        lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        lanternEmissionMapTexture->image2D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec2(lanternEmissionMapImage->getSize().x, lanternEmissionMapImage->getSize().y),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const gl::GLvoid*>(lanternEmissionMapImage->getPixelsPtr()));

        // TODO: extract this to material class
        auto lanternSpecularMapImage = std::make_unique<sf::Image>();

        if (!lanternSpecularMapImage->loadFromFile("media/lantern_specular.png"))
        {
            std::cerr << "[ERROR] Can not load texture" << std::endl;
            return 1;
        }

        lanternSpecularMapImage->flipVertically();

        auto lanternSpecularMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

        lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        lanternSpecularMapTexture->image2D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec2(lanternSpecularMapImage->getSize().x, lanternSpecularMapImage->getSize().y),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const gl::GLvoid*>(lanternSpecularMapImage->getPixelsPtr()));

        lanternEmissionMapImage.release();
        lanternSpecularMapImage.release();
    }

    std::shared_ptr<StaticModel> penModel = nullptr;
    std::unique_ptr<globjects::Texture> penNormalMapTexture = nullptr;

    {
        auto penNormalMapImage = std::make_unique<sf::Image>();

        if (!penNormalMapImage->loadFromFile("media/pen-normal.png"))
        {
            std::cerr << "[ERROR] Can not load texture" << std::endl;
            return 1;
        }

        penNormalMapImage->flipVertically();

        penNormalMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

        penNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        penNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        penNormalMapTexture->image2D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec2(penNormalMapImage->getSize().x, penNormalMapImage->getSize().y),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const gl::GLvoid*>(penNormalMapImage->getPixelsPtr()));

        penModel = AssimpModelLoader::staticModelFromFile("media/pen-lowpoly.obj", { "media" });

        // rotate -> scale -> translate; can be done as series of matrix multiplications M_translation * M_scale * M_rotation
        // each of the components, in turn, can also be a series of matrix multiplications: M_rotation = M_rotate_z * M_rotate_y * M_rotate_x
        penModel->setTransformation(
            glm::translate(glm::vec3(0.35f, 3.91f, -0.75f)) *
            glm::scale(glm::vec3(0.05f)) *
            (glm::rotate(glm::radians(12.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
        );

        penNormalMapImage.release();
    }

    auto scrollModel = AssimpModelLoader::staticModelFromFile("media/scroll.obj", { "media" });

    scrollModel->setTransformation(
        glm::translate(glm::vec3(0.0f, 3.85f, 0.0f)) *
        glm::scale(glm::vec3(0.5f))
    );

    std::unique_ptr<globjects::Texture> inkBottleNormalMapTexture  = nullptr;

    {
        auto inkBottleNormalMapImage = std::make_unique<sf::Image>();

        if (!inkBottleNormalMapImage->loadFromFile("media/ink-bottle-normal.png"))
        {
            std::cerr << "[ERROR] Can not load texture" << std::endl;
            return 1;
        }

        inkBottleNormalMapImage->flipVertically();

        inkBottleNormalMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

        inkBottleNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
        inkBottleNormalMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

        inkBottleNormalMapTexture->image2D(
            0,
            static_cast<gl::GLenum>(GL_RGBA8),
            glm::vec2(inkBottleNormalMapImage->getSize().x, inkBottleNormalMapImage->getSize().y),
            0,
            static_cast<gl::GLenum>(GL_RGBA),
            static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const gl::GLvoid*>(inkBottleNormalMapImage->getPixelsPtr()));
    }

    auto inkBottleModel = AssimpModelLoader::staticModelFromFile("media/ink-bottle.obj", { "media" });

    inkBottleModel->setTransformation(
        glm::translate(glm::vec3(-1.75f, 3.86f, 1.05f)) *
        glm::scale(glm::vec3(0.5f))
    );

    std::cout << "done" << std::endl;

    /*std::cout << "[DEBUG] Initializing skybox...";

    auto skybox = Skybox::builder()
        ->top("media/skybox-top.png")
        ->bottom("media/skybox-bottom.png")
        ->left("media/skybox-left.png")
        ->right("media/skybox-right.png")
        ->front("media/skybox-front.png")
        ->back("media/skybox-back.png")
        ->size(40.0f)
        ->build();

    std::cout << "done" << std::endl;*/

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing deferred rendering frame buffer...";

    auto deferredFragmentPositionTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentPositionTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentPositionTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentPositionTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB32F),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredFragmentNormalTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentNormalTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentNormalTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentNormalTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB32F),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredFragmentAlbedoTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentAlbedoTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentAlbedoTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentAlbedoTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        nullptr
    );

    auto deferredFragmentDepthTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    deferredFragmentDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    deferredFragmentDepthTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    deferredFragmentDepthTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr
    );

    auto deferredRenderingFramebuffer = std::make_unique<globjects::Framebuffer>();
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), deferredFragmentPositionTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1), deferredFragmentNormalTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT2), deferredFragmentAlbedoTexture.get());
    // deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT3), deferredFragmentLightSpacePositionTexture.get());
    deferredRenderingFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), deferredFragmentDepthTexture.get());

    // tell framebuffer it actually needs to render to **BOTH** textures, but does not have to output anywhere (last NONE argument, iirc)
    deferredRenderingFramebuffer->setDrawBuffers({
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1),
        static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT2),
        // static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT3),
        static_cast<gl::GLenum>(GL_NONE)
    });

    deferredRenderingFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing shadowMapTexture...";

    const float shadowMapSize = 2048.0f;

    auto shadowMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    shadowMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB10),
        glm::vec2(shadowMapSize, shadowMapSize),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing frame buffer...";

    auto shadowMapFramebuffer = std::make_unique<globjects::Framebuffer>();
    shadowMapFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), shadowMapTexture.get());
    shadowMapFramebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_NONE) });

    auto shadowMapRenderBuffer = std::make_unique<globjects::Renderbuffer>();
    shadowMapRenderBuffer->storage(static_cast<gl::GLenum>(GL_DEPTH24_STENCIL8), shadowMapSize, shadowMapSize);
    shadowMapFramebuffer->attachRenderBuffer(static_cast<gl::GLenum>(GL_DEPTH_STENCIL_ATTACHMENT), shadowMapRenderBuffer.get());

    shadowMapFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Preparing data buffers...";

    std::vector<PointLightDescriptor> pointLights{ { glm::vec3(-1.75f, 3.85f, -0.75f), 0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) } };

    auto pointLightDataBuffer = std::make_unique<globjects::Buffer>();

    pointLightDataBuffer->setData(pointLights, static_cast<gl::GLenum>(GL_DYNAMIC_COPY));

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

        // first render pass - prepare for deferred rendering by rendering to the entire scene to a deferred rendering framebuffer's attachments
        {
            deferredRenderingFramebuffer->bind();

            ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
            ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);

            deferredRenderingPrePassProgram->use();

            deferredRenderingPrePassProgram->setUniform("projection", cameraProjection);
            deferredRenderingPrePassProgram->setUniform("view", cameraView);

            deferredRenderingPrePassProgram->setUniform("diffuseTexture", 1);
            deferredRenderingPrePassProgram->setUniform("normalMapTexture", 2);

            deferredRenderingPrePassProgram->setUniform("model", houseModel->getTransformation());

            houseModel->bind();
            houseModel->draw();
            houseModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", tableModel->getTransformation());

            tableModel->bind();
            tableModel->draw();
            tableModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", lanternModel->getTransformation());

            lanternModel->bind();
            lanternModel->draw();
            lanternModel->unbind();

            deferredRenderingPrePassProgram->setUniform("model", penModel->getTransformation());

            penNormalMapTexture->bindActive(2);

            penModel->bind();
            penModel->draw();
            penModel->unbind();

            penNormalMapTexture->unbindActive(2);

            deferredRenderingPrePassProgram->setUniform("model", inkBottleModel->getTransformation());

            inkBottleNormalMapTexture->bindActive(2);

            inkBottleModel->bind();
            inkBottleModel->draw();
            inkBottleModel->unbind();

            inkBottleNormalMapTexture->unbindActive(2);

            deferredRenderingPrePassProgram->setUniform("model", scrollModel->getTransformation());

            glDisable(GL_CULL_FACE);

            scrollModel->bind();
            scrollModel->draw();
            scrollModel->unbind();

            glEnable(GL_CULL_FACE);

            deferredRenderingPrePassProgram->release();

            deferredRenderingFramebuffer->unbind();
        }

        // second render pass - merge textures from the deferred rendering pre-pass into a final frame
        {
            ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
            ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            deferredRenderingFinalPassProgram->use();

            deferredFragmentPositionTexture->bindActive(2);
            deferredFragmentNormalTexture->bindActive(3);
            deferredFragmentAlbedoTexture->bindActive(4);

            pointLightDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 5);

            deferredRenderingFinalPassProgram->setUniform("positionTexture", 2);
            deferredRenderingFinalPassProgram->setUniform("normalTexture", 3);
            deferredRenderingFinalPassProgram->setUniform("albedoTexture", 4);

            deferredRenderingFinalPassProgram->setUniform("cameraPosition", cameraPos);

            quadModel->bind();
            quadModel->draw();
            quadModel->unbind();

            pointLightDataBuffer->unbind(GL_SHADER_STORAGE_BUFFER, 5);

            deferredFragmentPositionTexture->unbindActive(2);
            deferredFragmentNormalTexture->unbindActive(3);
            deferredFragmentAlbedoTexture->unbindActive(4);

            deferredRenderingFinalPassProgram->release();
        }

        // done rendering the frame

        window.display();
    }

    return 0;
}
