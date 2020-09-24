#pragma once
#pragma comment(lib, "irrlicht.lib")

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <irrlicht.h>
#include <tinyxml2.h>

#include "CameraSceneNodeAnimator.h"
#include "GameData.h"
#include "GameManagerNodeData.h"
#include "GameManagerTree.h"
#include "GUIElementId.h"
#include "Level.h"

const std::wstring ABOUT_TEXT = L"This is the ShootThem! level editor.\n\n\
Use the menu to add new levels and targets to the level.\n\n\
Save and load levels from the levels.xml file that will be picked up by the game later on.";

class ApplicationDelegate {
public:
    ApplicationDelegate(irr::IrrlichtDevice* _device);

    void init();

    void update();

    void addTarget();

    void addLight();

    void saveLevels();

    void saveLevels(const std::wstring& filename);

    void loadLevels(const std::wstring& filename);

    void openSaveLevelsDialog();

    void closeSaveLevelsDialog();

    void openLoadLevelsDialog();

    void closeLoadLevelsDialog();

    void openAboutWindow();

    void closeAboutWindow();

    void openLoadLevelMeshDialog();

    void closeLoadLevelMeshDialog();

    void addLevel(const std::wstring& meshFilename);

    void levelSelected(const std::wstring& levelId);

    void targetSelected(const std::wstring& targetId);
    
    void lightSelected(const std::wstring& lightId);

    void gameManagerNodeSelected();
    
    void deleteSelectedEntity();

    void quit();

private:
    void initUI();

    void createToolbar();

    void createManagerWindow();

    void setFont();

    irr::core::vector3df getTargetPositionFromCameraView() const;

    irr::scene::ISceneNode* loadMesh(const std::wstring& meshFilename);

    irr::IrrlichtDevice* device;
    irr::video::IVideoDriver* driver;
    irr::scene::ISceneManager* smgr;
    irr::gui::IGUIEnvironment* guienv;
    irr::scene::ICameraSceneNode* camera;
    
    std::optional<std::wstring> levelsFilename;

    std::shared_ptr<GameData> gameData;
    std::shared_ptr<GameManagerTree> gameManagerTree;

    bool loadLevelsDialogIsShown;
    bool saveLevelsDialogIsShown;
    bool aboutWindowIsShown;
    bool loadLevelMeshDialogIsShown;
};
