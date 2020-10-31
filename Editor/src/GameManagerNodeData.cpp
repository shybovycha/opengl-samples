#include "GameManagerNodeData.h"

GameManagerNodeData::GameManagerNodeData(GameManagerNodeDataType _type, const std::wstring& _id) : type(_type), id(_id)
{
}

GameManagerNodeDataType GameManagerNodeData::getType() const
{
    return type;
}

std::wstring GameManagerNodeData::getId() const
{
    return id;
}
