#include "Target.h"

Target::Target(irr::core::vector3df _position, const std::wstring& _id) : LevelEntity(_position, _id, LevelEntityType::TARGET) {}
