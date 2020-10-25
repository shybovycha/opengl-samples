#include "ModernResourceManager.h"

ModernResourceManager::ModernResourceManager() : ResourceManager()
{
}

std::shared_ptr<Settings> ModernResourceManager::loadSettings()
{
    std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

    tinyxml2::XMLError xmlError = xml->LoadFile("data/settings.xml");

    if (xmlError != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Can not load settings.xml file" << std::endl;
        return nullptr;
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

    return std::make_shared<Settings>(driverName, resolutionWidth, resolutionHeight, colorDepth, fullScreen, vsync,
            stencil);
}

std::vector<std::shared_ptr<Level>> ModernResourceManager::loadLevels()
{
    std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

    tinyxml2::XMLError xmlError = xml->LoadFile("data/levels.xml");

    if (xmlError != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Can not load levels.xml file" << std::endl;
        return std::vector<std::shared_ptr<Level>>();
    }

    auto levelsNode = xml->FirstChildElement("levels");

    auto levelNode = levelsNode->FirstChildElement("level");

    std::vector<std::shared_ptr<Level>> levels;

    while (levelNode != nullptr)
    {
        std::string meshName = levelNode->FirstChildElement("model")->GetText();

        auto levelDescriptor = std::make_shared<Level>(meshName);

        auto entitiesNode = levelNode->FirstChildElement("entities");

        auto targetNode = entitiesNode->FirstChildElement("target");

        while (targetNode != nullptr)
        {
            auto positionNode = targetNode->FirstChildElement("position");

            Vector3<double> position{
                    positionNode->FloatAttribute("x", 0.0f),
                    positionNode->FloatAttribute("y", 0.0f),
                    positionNode->FloatAttribute("z", 0.0f)
            };

            levelDescriptor->addTargetPosition(position);

            targetNode = targetNode->NextSiblingElement("target");
        }

        auto lightNode = entitiesNode->FirstChildElement("light");

        while (lightNode != nullptr)
        {
            auto positionNode = lightNode->FirstChildElement("position");

            Vector3<double> position{
                    positionNode->FloatAttribute("x", 0.0f),
                    positionNode->FloatAttribute("y", 0.0f),
                    positionNode->FloatAttribute("z", 0.0f)
            };

            levelDescriptor->addLightPosition(position);

            lightNode = lightNode->NextSiblingElement("light");
        }

        levels.push_back(levelDescriptor);

        levelNode = levelNode->NextSiblingElement("level");
    }

    return levels;
}
