#include "Light.h"

Light::Light(const irr::core::vector3df& _position, const std::wstring& _id) : LevelEntity(_position, _id,
        LevelEntityType::LIGHT)
{
}
