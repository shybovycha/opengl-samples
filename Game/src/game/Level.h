#pragma once

#include <string>
#include <vector>

#include "../core/SceneNode.h"
#include "../core/Vector3.h"

class Level
{
public:
    Level(const std::string filename);

    std::string getModelFilename() const;

    void addTargetPosition(Vector3<double> position);

    void addLightPosition(Vector3<double> position);

    std::vector<Vector3<double>> getTargetPositions() const;

    std::vector<Vector3<double>> getLightPositions() const;

    void setModel(SceneNode* model);

    SceneNode* getModel() const;

    std::vector<SceneNode*> getTargets() const;

    std::vector<SceneNode*> getLights() const;

    void setTargets(std::vector<SceneNode*> targets);

    void setLights(std::vector<SceneNode*> lights);

private:
    std::string meshFilename;

    std::vector<Vector3<double>> targetPositions;
    std::vector<Vector3<double>> lightPositions;
    std::vector<SceneNode*> targets;
    std::vector<SceneNode*> lights;

    SceneNode* model;
};
