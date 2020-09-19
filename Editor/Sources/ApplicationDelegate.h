#pragma once
#pragma comment(lib, "irrlicht.lib")

#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

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
    ApplicationDelegate(std::shared_ptr<irr::IrrlichtDevice> _device);

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

    std::shared_ptr<irr::IrrlichtDevice> device;
    std::shared_ptr<irr::video::IVideoDriver> driver;
    std::shared_ptr<irr::scene::ISceneManager> smgr;
    std::shared_ptr<irr::gui::IGUIEnvironment> guienv;
    std::shared_ptr<irr::scene::ICameraSceneNode> camera;
    
    std::optional<std::wstring> levelsFilename;

    std::vector<std::shared_ptr<Level>> levels;

    size_t currentLevelIndex;

    bool loadLevelsDialogIsShown;
    bool saveLevelsDialogIsShown;
    bool aboutWindowIsShown;
    bool loadLevelMeshDialogIsShown;
};
