#pragma once

#include <filesystem>
#include <iostream>

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

#include "Mesh.hpp"

class Model
{
public:
    Model(std::vector<std::unique_ptr<Mesh>> meshes);

    static std::unique_ptr<Model> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {});

    void setTransformation(glm::mat4 transformation);

    glm::mat4 getTransformation() const;

    void draw();

    void bind();

    void unbind();

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<Mesh>>& meshes);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    glm::mat4 m_transformation;
};
