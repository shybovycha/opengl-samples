#include "GameManagerNodeData.h"

#include <utility>

GameManagerNodeData::GameManagerNodeData(GameManagerNodeDataType _type, std::wstring  _id) : type(_type), id(std::move(_id)) {}

GameManagerNodeDataType GameManagerNodeData::getType() const {
    return type;
}

std::wstring GameManagerNodeData::getId() const {
    return id;
}
