#pragma once

class PlayerState
{
public:
    PlayerState();

    void setMaxAmmo(unsigned int _maxAmmo);

    void reload();

    void shoot();

    const unsigned int getCurrentAmmo() const;

    const unsigned int getMaxAmmo() const;

private:
    unsigned int currentAmmo;
    unsigned int maxAmmo;
};
