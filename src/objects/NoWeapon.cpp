//
// Created by jprolejko on 21.11.19.
//

#include <objects/NoWeapon.h>

NoWeapon::NoWeapon() : AbstractWeapon({}, "") {}

sf::Vector2f NoWeapon::use() {
    return {0.0f, 0.0f};
}