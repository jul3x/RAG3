//
// Created by jul3x on 16.10.19.
//

#include <game/weapons/AbstractWeapon.h>
#include <engine/system/AbstractResourceManager.h>


AbstractWeapon::AbstractWeapon(const sf::Vector2f& size,
                               const std::string& name) :
        name_(name),
        AbstractDrawableObject({}, size,
                               name.empty() ? nullptr : &AbstractResourceManager::getInstance().getTexture("weapon_" + name))
{
}

const std::string& AbstractWeapon::getName() const
{
    return name_;
}