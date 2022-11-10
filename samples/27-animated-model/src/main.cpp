#include "common/stdafx.hpp"

void processSceneNode(aiNode* node)
{
    for (auto childIdx = 0; childIdx < node->mNumChildren; ++childIdx)
    {
        processSceneNode(node->mChildren[childIdx]);
    }
}

struct Keyframe
{
    aiVector3D position;
    aiQuaternion rotation;
    aiVector3D scale;
};

void loadAnimationsFromFile(std::string filename)
{
    Assimp::Importer importer;

    auto scene = importer.ReadFile(filename, aiProcess_PopulateArmatureData);

    std::cout << "Found skeletons: " << scene->mNumSkeletons << std::endl;

    // cache all bones
    std::map<std::string, aiBone*> bones;

    for (auto i = 0; i < scene->mNumMeshes; ++i)
    {
        auto mesh = scene->mMeshes[i];

        for (auto t = 0; t < mesh->mNumBones; ++t)
        {
            auto bone = mesh->mBones[t];
            auto boneName = bone->mName.C_Str();

            bones[boneName] = bone;
        }
    }

    // iterate over animations and print out channels
    for (auto i = 0; i < scene->mNumAnimations; ++i)
    {
        auto animation = scene->mAnimations[i];

        std::cout << "Animation \"" << animation->mName.C_Str() << "\" [ " << animation->mDuration << " ticks / " << (animation->mDuration / animation->mTicksPerSecond) << " sec]" << std::endl;

        for (auto t = 0; t < animation->mNumChannels; ++t)
        {
            auto channel = animation->mChannels[t];

            // cache keyframes
            std::map<double, Keyframe> keyframes;

            // position keyframes
            for (auto j = 0; j < channel->mNumPositionKeys; ++j)
            {
                auto positionKey = channel->mPositionKeys[j];

                if (keyframes.find(positionKey.mTime) != keyframes.end())
                {
                    keyframes[positionKey.mTime].position = positionKey.mValue;
                }
                else
                {
                    keyframes[positionKey.mTime] = { .position = positionKey.mValue };
                }
            }

            // rotation keyframes
            for (auto j = 0; j < channel->mNumRotationKeys; ++j)
            {
                auto rotationKey = channel->mRotationKeys[j];

                if (keyframes.find(rotationKey.mTime) != keyframes.end())
                {
                    keyframes[rotationKey.mTime].rotation = rotationKey.mValue;
                }
                else
                {
                    keyframes[rotationKey.mTime] = { .rotation = rotationKey.mValue };
                }
            }

            // scale keyframes
            for (auto j = 0; j < channel->mNumScalingKeys; ++j)
            {
                auto scalingKey = channel->mScalingKeys[j];

                if (keyframes.find(scalingKey.mTime) != keyframes.end())
                {
                    keyframes[scalingKey.mTime].scale = scalingKey.mValue;
                }
                else
                {
                    keyframes[scalingKey.mTime] = { .scale = scalingKey.mValue };
                }
            }

            // print out keyframes for a given bone
            for (auto& keyframe : keyframes)
            {
                auto nodeName = channel->mNodeName.C_Str();
                auto boneFound = bones.find(nodeName) != bones.end();

                std::cout
                    << "<bone \"" << nodeName << "\", " << (boneFound ? "found" : "not found") << ">"
                    << "[ " << keyframe.first << " ] = { "
                    << "pos : (" << keyframe.second.position.x << "," << keyframe.second.position.y << ", " << keyframe.second.position.z << "); "
                    << "rot : (" << keyframe.second.rotation.x << "," << keyframe.second.rotation.y << ", " << keyframe.second.rotation.z << ", " << keyframe.second.rotation.w << "); "
                    << "sca : (" << keyframe.second.scale.x << "," << keyframe.second.scale.y << ", " << keyframe.second.scale.z << ")"
                    << " }" << std::endl;
            }
        }
    }
}

int main()
{
    loadAnimationsFromFile("media/dancing-cactus.gltf");

     return 0;
}
