#include "GameData.h"

GameData::GameData(irr::IrrlichtDevice* _device) : device(_device)
{
}

void GameData::loadFromFile(const std::wstring& filename)
{
    auto xml = new tinyxml2::XMLDocument();

    // setup wstring -> string converter
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    tinyxml2::XMLError xmlError = xml->LoadFile(wstringConverter.to_bytes(filename).c_str());

    if (xmlError != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Can not load levels.xml file" << std::endl;
        return;
    }

    auto levelsNode = xml->FirstChildElement("levels");

    auto levelNode = levelsNode->FirstChildElement("level");
    auto lastLevelNode = levelsNode->LastChildElement("level");

    levels.clear();

    while (levelNode != nullptr)
    {
        std::string meshName = levelNode->FirstChildElement("model")->GetText();

        std::wostringstream levelIdString;
        levelIdString << "level-" << levels.size();

        auto levelDescriptor = std::make_shared<Level>(wstringConverter.from_bytes(meshName.c_str()),
                levelIdString.str());

        auto entitiesNode = levelNode->FirstChildElement("entities");

        auto currentTargetNode = entitiesNode->FirstChildElement("target");
        auto lastTargetNode = entitiesNode->LastChildElement("target");

        while (currentTargetNode != nullptr)
        {
            auto positionNode = currentTargetNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f),
                    positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->createTarget(position);

            currentTargetNode = currentTargetNode->NextSiblingElement("target");
        }

        auto currentLightNode = entitiesNode->FirstChildElement("light");
        auto lastLightNode = entitiesNode->LastChildElement("light");

        while (currentLightNode != nullptr)
        {
            auto positionNode = currentLightNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f),
                    positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->createLight(position);

            currentLightNode = currentLightNode->NextSiblingElement("light");
        }

        levels[levelIdString.str()] = levelDescriptor;

        levelNode = levelNode->NextSiblingElement("level");
    }
}

void GameData::saveToFile(const std::wstring& filename)
{
    // setup wstring -> string converter
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    std::FILE* fp = std::fopen(wstringConverter.to_bytes(filename).c_str(), "w");
    auto writer = new tinyxml2::XMLPrinter(fp);

    writer->OpenElement("levels");

    for (auto const& it : levels)
    {
        std::shared_ptr<Level> level = it.second;

        writer->OpenElement("level");

        writer->OpenElement("model");

        std::wstring meshFilename = level->getMeshBasename();

        writer->PushText(wstringConverter.to_bytes(meshFilename).c_str());

        writer->CloseElement();

        writer->OpenElement("entities");

        for (const auto& entity : level->getEntities())
        {
            irr::core::vector3df position = entity->getPosition();

            if (entity->getType() == LevelEntityType::TARGET)
            {
                writer->OpenElement("target");
            }
            else
            {
                writer->OpenElement("light");
            }

            writer->OpenElement("position");

            writer->PushAttribute("x", position.X);
            writer->PushAttribute("y", position.Y);
            writer->PushAttribute("z", position.Z);

            writer->CloseElement(); // position

            if (entity->getType() == LevelEntityType::TARGET)
            {
                writer->CloseElement("target");
            }
            else
            {
                writer->CloseElement("light");
            }
        }

        writer->CloseElement(); // entities

        writer->CloseElement(); // level
    }

    writer->CloseElement(); // levels
}

std::shared_ptr<Level> GameData::getLevelById(const std::wstring& levelId) const
{
    return levels.at(levelId);
}

std::shared_ptr<Level> GameData::createLevel(const std::wstring& meshFilename)
{
    std::wostringstream idString;
    idString << L"level-" << levels.size();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    irr::io::path path = device->getFileSystem()->getFileBasename(meshFilename.c_str());
    std::wstring meshBasename = wstringConverter.from_bytes(path.c_str());

    auto level = std::make_shared<Level>(meshFilename, idString.str(), meshBasename);

    levels[idString.str()] = level;

    return level;
}

void GameData::deleteLevelById(const std::wstring& levelId)
{
    auto level = levels.at(levelId);

    if (level == nullptr)
    {
        return;
    }

    if (level->getSceneNode() != nullptr)
    {
        level->getSceneNode()->remove();
        level->setSceneNode(nullptr);
    }

    levels.erase(levelId);
}

std::vector<std::shared_ptr<Level>> GameData::getLevels() const
{
    std::vector<std::shared_ptr<Level>> result;

    for (auto const& it : levels)
    {
        result.push_back(it.second);
    }

    return result;
}

std::shared_ptr<Level> GameData::getCurrentLevel() const
{
    return currentLevel;
}

std::shared_ptr<LevelEntity> GameData::getCurrentEntity() const
{
    return currentEntity;
}

void GameData::setCurrentLevel(std::shared_ptr<Level> level)
{
    currentLevel = level;
}

void GameData::setCurrentEntity(std::shared_ptr<LevelEntity> entity)
{
    currentEntity = entity;
}
