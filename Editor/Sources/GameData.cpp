#include "GameData.h"

GameData::GameData(irr::IrrlichtDevice* _device) : device(_device) {}

void GameData::loadFromFile(const std::wstring& filename) {
    tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();

    // setup wstring -> string converter
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    tinyxml2::XMLError xmlError = xml->LoadFile(wstringConverter.to_bytes(filename).c_str());

    if (xmlError != tinyxml2::XML_SUCCESS) {
        std::cerr << "Can not load levels.xml file" << std::endl;
        throw "Can not load levels";
    }

    auto levelsNode = xml->FirstChildElement("levels");

    auto levelNode = levelsNode->FirstChildElement("level");
    auto lastLevelNode = levelsNode->LastChildElement("level");

    levels.clear();

    while (levelNode != nullptr) {
        std::string meshName = levelNode->FirstChildElement("model")->GetText();

        std::wostringstream levelIdString;
        levelIdString << "level-" << levels.size();

        auto levelDescriptor = std::make_shared<Level>(wstringConverter.from_bytes(meshName.c_str()), levelIdString.str());

        auto targetsNode = levelNode->FirstChildElement("targets");

        auto currentTargetNode = targetsNode->FirstChildElement("target");
        auto lastTargetNode = targetsNode->LastChildElement("target");

        while (currentTargetNode != nullptr) {
            auto positionNode = currentTargetNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f), positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->createTarget(position);

            currentTargetNode = currentTargetNode->NextSiblingElement("target");
        }

        auto lightsNode = levelNode->FirstChildElement("lights");

        auto currentLightNode = lightsNode->FirstChildElement("light");
        auto lastLightNode = lightsNode->LastChildElement("light");

        while (currentLightNode != nullptr) {
            auto positionNode = currentLightNode->FirstChildElement("position");

            irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f), positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

            levelDescriptor->createLight(position);

            currentLightNode = currentLightNode->NextSiblingElement("light");
        }

        levels.push_back(levelDescriptor);

        levelNode = levelNode->NextSiblingElement("level");
    }
}

void GameData::saveToFile(const std::wstring& filename) {
    // setup wstring -> string converter
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    std::FILE* fp = std::fopen(wstringConverter.to_bytes(filename).c_str(), "w");
    tinyxml2::XMLPrinter* writer = new tinyxml2::XMLPrinter(fp);

    writer->OpenElement("levels");

    for (std::shared_ptr<Level> level : levels) {
        writer->OpenElement("level");

        writer->OpenElement("model");

        std::wstring meshFilename = level->getMeshBasename();

        writer->PushText(wstringConverter.to_bytes(meshFilename).c_str());

        writer->CloseElement();

        writer->OpenElement("targets");

        for (auto target : level->getTargets()) {
            irr::core::vector3df position = target->getPosition();

            writer->OpenElement("target");

            writer->OpenElement("position");

            writer->PushAttribute("x", position.X);
            writer->PushAttribute("y", position.Y);
            writer->PushAttribute("z", position.Z);

            writer->CloseElement(); // position

            writer->CloseElement(); // target
        }

        writer->CloseElement(); // targets

        writer->OpenElement("lights");

        for (auto light : level->getLights()) {
            irr::core::vector3df position = light->getPosition();

            writer->OpenElement("light");

            writer->OpenElement("position");

            writer->PushAttribute("x", position.X);
            writer->PushAttribute("y", position.Y);
            writer->PushAttribute("z", position.Z);

            writer->CloseElement(); // position

            writer->CloseElement(); // light
        }

        writer->CloseElement(); // lights

        writer->CloseElement(); // level
    }

    writer->CloseElement(); // levels
}

std::shared_ptr<Level> GameData::getLevelById(const std::wstring& levelId) const {
    auto it = std::find_if(levels.begin(), levels.end(), [&](const std::shared_ptr<Level> level) { return level->getId() == levelId; });

    if (it == levels.end()) {
        return nullptr;
    }

    return *it;
}

std::shared_ptr<Level> GameData::createLevel(const std::wstring& meshFilename) {
    std::wostringstream idString;
    idString << L"level-" << levels.size();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstringConverter;

    irr::io::path path = device->getFileSystem()->getFileBasename(meshFilename.c_str());
    std::wstring meshBasename = wstringConverter.from_bytes(path.c_str());

    auto level = std::make_shared<Level>(meshFilename, idString.str(), meshBasename);

    levels.push_back(level);

    return level;
}

std::vector<std::shared_ptr<Level>> GameData::getLevels() const {
    return levels;
}

std::shared_ptr<Level> GameData::getCurrentLevel() const {
    return currentLevel;
}

std::shared_ptr<Target> GameData::getCurrentTarget() const {
    return currentTarget;
}

void GameData::setCurrentLevel(std::shared_ptr<Level> level) {
    currentLevel = level;
}

void GameData::setCurrentTarget(std::shared_ptr<Target> target) {
    currentTarget = target;
}

std::shared_ptr<Light> GameData::getCurrentLight() const {
    return currentLight;
}

void GameData::setCurrentLight(std::shared_ptr<Light> light) {
    currentLight = light;
}
