#include "Light.h"

Light::Light(irr::core::vector3df _position, const std::wstring& _id) : LevelEntity(_position, _id, LevelEntityType::LIGHT) {}
