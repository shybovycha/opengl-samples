#include "GameManagerTree.h"

GameManagerTree::GameManagerTree(irr::gui::IGUIEnvironment* _guienv, std::shared_ptr<GameData> _gameData) : guienv(
        _guienv), gameData(_gameData), gameManagerTree(nullptr)
{
}

void GameManagerTree::init()
{
    gameManagerTree = reinterpret_cast<irr::gui::IGUITreeView*>(guienv->getRootGUIElement()->getElementFromId(
            static_cast<irr::s32>(GUIElementId::GAME_LEVEL_TREE), true));
}

void GameManagerTree::rebuild()
{
    gameManagerTree->getRoot()->clearChildren();

    for (const auto& level : gameData->getLevels())
    {
        auto levelNodeData = new GameManagerNodeData(GameManagerNodeDataType::LEVEL, level->getId());

        auto levelTreeNode = addManagerTreeNodeToRootNode(level->getMeshBasename(), levelNodeData);

        if (gameData->getCurrentLevel() != nullptr && gameData->getCurrentEntity() == nullptr &&
            gameData->getCurrentLevel()->getId() == level->getId())
        {
            levelTreeNode->setSelected(true);
            levelTreeNode->setExpanded(true);
        }

        for (const auto& entity : level->getEntities())
        {
            GameManagerNodeData* entityNodeData = nullptr;
            std::wstring id = entity->getId();

            if (entity->getType() == LevelEntityType::LIGHT)
            {
                entityNodeData = new GameManagerNodeData(GameManagerNodeDataType::LIGHT, id);
            }
            else if (entity->getType() == LevelEntityType::TARGET)
            {
                entityNodeData = new GameManagerNodeData(GameManagerNodeDataType::TARGET, id);
            }

            auto entityTreeNode = addManagerTreeNodeToNode(id, entityNodeData, levelTreeNode);

            if (gameData->getCurrentEntity() != nullptr && gameData->getCurrentEntity()->getId() == id)
            {
                entityTreeNode->setSelected(true);
                levelTreeNode->setExpanded(true);
            }
        }
    }
}

irr::gui::IGUITreeViewNode*
GameManagerTree::addManagerTreeNodeToRootNode(const std::wstring& label, GameManagerNodeData* nodeData)
{
    return addManagerTreeNodeToNode(label, nodeData, gameManagerTree->getRoot());
}

irr::gui::IGUITreeViewNode*
GameManagerTree::addManagerTreeNodeToSelectedNode(const std::wstring& label, GameManagerNodeData* nodeData)
{
    irr::gui::IGUITreeViewNode* selectedNode = gameManagerTree->getSelected();

    if (!selectedNode)
    {
        selectedNode = gameManagerTree->getRoot();
    }

    return addManagerTreeNodeToNode(label, nodeData, selectedNode);
}

irr::gui::IGUITreeViewNode*
GameManagerTree::addManagerTreeNodeToNode(const std::wstring& label, GameManagerNodeData* nodeData,
        irr::gui::IGUITreeViewNode* parent)
{
    return parent->addChildBack(label.c_str(), nullptr, -1, -1, reinterpret_cast<void*>(nodeData));
}

GameManagerNodeData* GameManagerTree::getSelectedNodeData() const
{
    auto selectedNode = gameManagerTree->getSelected();

    if (!selectedNode)
    {
        return nullptr;
    }

    return reinterpret_cast<GameManagerNodeData*>(selectedNode->getData());
}
