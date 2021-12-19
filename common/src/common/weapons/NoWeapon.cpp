
//
// Created by jul3x on 21.11.19.
//

#include <common/weapons/NoWeapon.h>
#include <common/characters/Character.h>


inline bool shouldDrawHand(const std::string& id)
{
    return utils::contains(CONF<j3x::List>("multi_to_play"), id);
}

NoWeapon::NoWeapon(Character* user) :
        AbstractWeapon(user,
                       shouldDrawHand(user->getId()) ? sf::Vector2f(16.0f, 12.0f) : sf::Vector2f(),
                       sf::Vector2f(),
                       shouldDrawHand(user->getId()) ? "null" : "")
{
    this->changeOrigin(sf::Vector2f(6.0f, 12.0) / 2.0f);
}

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

void NoWeapon::recalculate()
{
}
