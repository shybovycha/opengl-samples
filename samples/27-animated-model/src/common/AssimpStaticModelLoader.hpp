#pragma once

#include "stdafx.hpp"

#include "StaticMesh.hpp"
#include "StaticModel.hpp"

class AssimpStaticModelLoader
{
public:
    AssimpStaticModelLoader();

    static std::unique_ptr<StaticModel> fromFile(std::string filename, std::vector<std::filesystem::path> materialLookupPaths = {}, unsigned int assimpImportFlags = 0);

protected:
    static std::unique_ptr<StaticModel> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {});

    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<StaticMesh>>& meshes);

    static std::unique_ptr<StaticMesh> fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {});
};
