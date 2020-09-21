#include "GameManagerTree.h"

GameManagerTree::GameManagerTree(irr::gui::IGUIEnvironment* _guienv, std::shared_ptr<GameData> _gameData) : guienv(_guienv), gameData(_gameData) {}

void GameManagerTree::init() {
    gameManagerTree = reinterpret_cast<irr::gui::IGUITreeView*>(guienv->getRootGUIElement()->getElementFromId(static_cast<irr::s32>(GUIElementId::GAME_LEVEL_TREE), true));
}

void GameManagerTree::rebuild() {
    gameManagerTree->getRoot()->clearChildren();

    for (auto level : gameData->getLevels()) {
        GameManagerNodeData* levelNodeData = new GameManagerNodeData(GameManagerNodeDataType::LEVEL, level->getId());

        auto levelTreeNode = addManagerTreeNodeToRootNode(level->getMeshBasename().c_str(), levelNodeData);

        if (gameData->getCurrentLevel() != nullptr && gameData->getCurrentTarget() == nullptr && gameData->getCurrentLevel()->getId() == level->getId()) {
            levelTreeNode->setSelected(true);
            levelTreeNode->setExpanded(true);
        }

        for (auto target : level->getTargets()) {
            GameManagerNodeData* targetNodeData = new GameManagerNodeData(GameManagerNodeDataType::TARGET, target->getId());

            auto targetTreeNode = addManagerTreeNodeToNode(target->getId(), targetNodeData, levelTreeNode);

            if (gameData->getCurrentTarget() != nullptr && gameData->getCurrentTarget()->getId() == target->getId()) {
                targetTreeNode->setSelected(true);
                levelTreeNode->setExpanded(true);
            }
        }
    }
}

irr::gui::IGUITreeViewNode* GameManagerTree::addManagerTreeNodeToRootNode(std::wstring label, GameManagerNodeData* nodeData) {
    return addManagerTreeNodeToNode(label, nodeData, gameManagerTree->getRoot());
}

irr::gui::IGUITreeViewNode* GameManagerTree::addManagerTreeNodeToSelectedNode(std::wstring label, GameManagerNodeData* nodeData) {
    irr::gui::IGUITreeViewNode* selectedNode = gameManagerTree->getSelected();

    if (!selectedNode) {
        selectedNode = gameManagerTree->getRoot();
    }

    return addManagerTreeNodeToNode(label, nodeData, selectedNode);
}

irr::gui::IGUITreeViewNode* GameManagerTree::addManagerTreeNodeToNode(std::wstring label, GameManagerNodeData* nodeData, irr::gui::IGUITreeViewNode* parent) {
    return parent->addChildBack(label.c_str(), nullptr, -1, -1, reinterpret_cast<void*>(nodeData));
}

GameManagerNodeData* GameManagerTree::getSelectedNodeData() const {
    auto selectedNode = gameManagerTree->getSelected();

    if (!selectedNode) {
        return nullptr;
    }

    auto selectedNodeData = reinterpret_cast<GameManagerNodeData*>(selectedNode->getData());

    if (selectedNodeData->getType() == GameManagerNodeDataType::LEVEL) {
        gameData->setCurrentLevel(gameData->getLevelById(selectedNodeData->getId()));
    }
    else if (selectedNodeData->getType() == GameManagerNodeDataType::TARGET) {
        gameData->setCurrentTarget(gameData->getCurrentLevel()->getTargetById(selectedNodeData->getId()));
    }

    return selectedNodeData;
}
