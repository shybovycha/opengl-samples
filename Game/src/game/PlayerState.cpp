#include "PlayerState.h"

PlayerState::PlayerState() : currentAmmo(10), maxAmmo(10)
{
}

void PlayerState::setMaxAmmo(unsigned int _maxAmmo)
{
    maxAmmo = _maxAmmo;
}

void PlayerState::reload()
{
    if (currentAmmo < maxAmmo)
    {
        currentAmmo = maxAmmo;
    }
}

void PlayerState::shoot()
{
    if (currentAmmo > 0)
    {
        currentAmmo--;
    }
}

const unsigned int PlayerState::getCurrentAmmo() const
{
    return currentAmmo;
}

const unsigned int PlayerState::getMaxAmmo() const
{
    return maxAmmo;
}
