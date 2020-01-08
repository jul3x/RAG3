//
// Created by jul3x on 16.10.19.
//

#include <game/weapons/AbstractWeapon.h>
#include <game/misc/ResourceManager.h>


AbstractWeapon::AbstractWeapon(const sf::Vector2f& size,
                               const std::string& name) :
        name_(name),
        AbstractDrawableObject({}, size,
                               name.empty() ? nullptr : &ResourceManager::getInstance().getTexture("weapon_" + name))
{
}

const std::string& AbstractWeapon::getName() const
{
    return name_;
}