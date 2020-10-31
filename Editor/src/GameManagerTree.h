#pragma once

#include <memory>
#include <string>
#include <sstream>

#include <irrlicht.h>

#include "GameData.h"
#include "GameManagerNodeData.h"
#include "GUIElementId.h"

class GameManagerTree
{
public:
    GameManagerTree(irr::gui::IGUIEnvironment* guienv, std::shared_ptr<GameData> gameData);

    void init();

    void rebuild();

    GameManagerNodeData* getSelectedNodeData() const;

private:
    irr::gui::IGUITreeViewNode* addManagerTreeNodeToRootNode(const std::wstring& label, GameManagerNodeData* nodeData);

    irr::gui::IGUITreeViewNode*
    addManagerTreeNodeToSelectedNode(const std::wstring& label, GameManagerNodeData* nodeData);

    irr::gui::IGUITreeViewNode* addManagerTreeNodeToNode(const std::wstring& label, GameManagerNodeData* nodeData,
            irr::gui::IGUITreeViewNode* parent);

    irr::gui::IGUIEnvironment* guienv;
    std::shared_ptr<GameData> gameData;
    irr::gui::IGUITreeView* gameManagerTree;
};
