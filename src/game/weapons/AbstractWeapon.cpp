//
// Created by jul3x on 16.10.19.
//

#include <game/weapons/AbstractWeapon.h>
#include <game/misc/ResourceManager.h>


AbstractWeapon::AbstractWeapon(const sf::Vector2f& size,
                               const sf::Vector2f& weapon_offset,
                               const std::string& name) :
        name_(name),
        weapon_offset_(weapon_offset),
        AbstractDrawableObject({}, size,
                               name.empty() ? nullptr : &ResourceManager::getInstance().getTexture("weapon_" + name))
{
}

const std::string& AbstractWeapon::getName() const
{
    return name_;
}

const sf::Vector2f& AbstractWeapon::getWeaponOffset() const
{
    return weapon_offset_;
}