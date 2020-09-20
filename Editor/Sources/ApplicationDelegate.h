#pragma once
#pragma comment(lib, "irrlicht.lib")

#include <codecvt> // for converting wstring to string
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <xlocbuf> // for converting wstring to string

#include "irrlicht.h"
#include "tinyxml2.h"

#include "Level.h"
#include "GameManagerNodeData.h"

const std::wstring ABOUT_TEXT = L"This is the ShootThem! level editor.\n\n\
Use the menu to add new levels and targets to the level.\n\n\
Save and load levels from the levels.xml file that will be picked up by the game later on.";

enum class GuiElementID {
    LOAD_LEVELS,
    SAVE_LEVELS,
    ABOUT,
    QUIT,
    ADD_LEVEL,
    ADD_TARGET,
    SAVE_LEVELS_DIALOG,
    LOAD_LEVELS_DIALOG,
    ABOUT_DIALOG,
    MANAGER_WINDOW,
    GAME_LEVEL_TREE,
    LOAD_LEVEL_MESH_DIALOG
};

class ApplicationDelegate {
public:
    ApplicationDelegate(irr::IrrlichtDevice* _device);

    void init();

    void update();

    void placeTarget();

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

    void quit();

private:
    void initUI();

    void createToolbar();

    void createManagerWindow();

    irr::gui::IGUITreeView* getGameTreeView();

    void addManagerTreeNodeToRootNode(std::wstring label, GameManagerNodeData* nodeData);

    void addManagerTreeNodeToSelectedNode(std::wstring label, GameManagerNodeData* nodeData);

    void setFont();

    irr::IrrlichtDevice* device;
    irr::video::IVideoDriver* driver;
    irr::scene::ISceneManager* smgr;
    irr::gui::IGUIEnvironment* guienv;
    irr::scene::ICameraSceneNode* camera;
    
    std::optional<std::wstring> levelsFilename;

    std::vector<std::shared_ptr<Level>> levels;

    size_t currentLevelIndex;

    bool loadLevelsDialogIsShown;
    bool saveLevelsDialogIsShown;
    bool aboutWindowIsShown;
    bool loadLevelMeshDialogIsShown;
};
