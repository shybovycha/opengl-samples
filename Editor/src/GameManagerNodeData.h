#pragma once

#include <string>

enum class GameManagerNodeDataType
{
    LEVEL,
    TARGET,
    LIGHT,
    PLAYER
};

class GameManagerNodeData
{
public:
    GameManagerNodeData(GameManagerNodeDataType _type, const std::wstring& _id);

    GameManagerNodeDataType getType() const;

    std::wstring getId() const;

protected:
    GameManagerNodeDataType type;
    std::wstring id;
};
