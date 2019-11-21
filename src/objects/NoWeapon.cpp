//
// Created by jprolejko on 21.11.19.
//

#include <objects/NoWeapon.h>

NoWeapon::NoWeapon() : Weapon(0.0f, 0.0f, 0, {}, {}, "", 0, 0.0f, "noweapon") {}

sf::Vector2f NoWeapon::shot() {
    return {0.0f, 0.0f};
}