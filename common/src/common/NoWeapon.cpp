
//
// Created by jul3x on 21.11.19.
//

#include <common/NoWeapon.h>


NoWeapon::NoWeapon() : AbstractWeapon(nullptr, {}, {}, "") {}

sf::Vector2f NoWeapon::use()
{
    return {0.0f, 0.0f};
}

float NoWeapon::getState() const
{
    return 0.0f;
}

void NoWeapon::setState(float state)
{
}