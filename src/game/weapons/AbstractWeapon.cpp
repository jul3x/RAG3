//
// Created by jprolejko on 16.10.19.
//

#include <game/weapons/AbstractWeapon.h>


AbstractWeapon::AbstractWeapon(const sf::Vector2f &size,
                               const std::string &name) :
    name_(name),
    AbstractDrawableObject({}, size, name == "" ? "" : "weapon_" + name) {
}

const std::string& AbstractWeapon::getName() const {
    return name_;
}