#pragma once

#include "stdafx.hpp"

#include "AbstractMesh.hpp"
#include "AbstractMeshBuilder.hpp"
#include "MultiMeshModel.hpp"

class AssimpModel : public MultiMeshModel
{
public:
    AssimpModel(std::vector<std::unique_ptr<AbstractMesh>> meshes);

    static std::unique_ptr<AssimpModel> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {});

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<AbstractMesh>>& meshes);

    static std::unique_ptr<AbstractMesh> fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {});
};
