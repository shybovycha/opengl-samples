#include "ModernResourceManager.h"

ModernResourceManager::ModernResourceManager() : ResourceManager() {}

Settings ModernResourceManager::loadSettings() {
    std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

    tinyxml2::XMLError xmlError = xml->LoadFile("Data/settings.xml");

    if (xmlError != tinyxml2::XML_SUCCESS) {
        std::cerr << "Can not load settings.xml file" << std::endl;
        throw std::exception("Can not load settings");
    }

    auto settingsNode = xml->FirstChildElement("settings");
    auto graphicsSettingsNode = settingsNode->FirstChildElement("graphics");

    std::string driverName = graphicsSettingsNode->FirstChildElement("driver")->GetText();

    int resolutionWidth = graphicsSettingsNode->FirstChildElement("resolution")->IntAttribute("width", 640);
    int resolutionHeight = graphicsSettingsNode->FirstChildElement("resolution")->IntAttribute("height", 480);
    int colorDepth = graphicsSettingsNode->FirstChildElement("colorDepth")->IntText(16);

    bool fullScreen = graphicsSettingsNode->FirstChildElement("fullScreen")->BoolText(false);
    bool vsync = graphicsSettingsNode->FirstChildElement("vsync")->BoolText(false);
    bool stencil = graphicsSettingsNode->FirstChildElement("stencilBuffer")->BoolText(false);

    return Settings{ driverName, resolutionWidth, resolutionHeight, colorDepth, fullScreen, vsync, stencil };
}

std::vector<std::shared_ptr<Level>> ModernResourceManager::loadLevels() {
    std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

    tinyxml2::XMLError xmlError = xml->LoadFile("Data/levels.xml");

    if (xmlError != tinyxml2::XML_SUCCESS) {
        std::cerr << "Can not load levels.xml file" << std::endl;
        throw std::exception("Can not load levels");
    }

    auto levelsNode = xml->FirstChildElement("levels");

    auto levelNode = levelsNode->FirstChildElement("level");
    auto lastLevelNode = levelsNode->LastChildElement("level");

    std::vector<std::shared_ptr<Level>> levels;

    while (levelNode != nullptr) {
        std::string meshName = levelNode->FirstChildElement("model")->GetText();

        auto levelDescriptor = std::make_shared<Level>(meshName);

        auto entitiesNode = levelNode->FirstChildElement("entities");

        auto targetNode = entitiesNode->FirstChildElement("target");
        auto lastTargetNode = entitiesNode->LastChildElement("target");

        while (targetNode != lastTargetNode) {
            auto positionNode = targetNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f), positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->addTargetPosition(position);

            targetNode = targetNode->NextSiblingElement("target");
        }

        auto lightNode = entitiesNode->FirstChildElement("light");
        auto lastLightNode = entitiesNode->LastChildElement("light");

        while (lightNode != lastLightNode) {
            auto positionNode = lightNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f), positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->addLightPosition(position);

            lightNode = lightNode->NextSiblingElement("light");
        }

        levels.push_back(levelDescriptor);

        levelNode = levelNode->NextSiblingElement("level");
    }

    return levels;
}
