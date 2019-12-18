//
// Created by jprolejko on 03.11.19.
//

#include <engine/system/ResourceManager.h>

#include <game/characters/Player.h>
#include <game/weapons/ShootingWeapon.h>
#include <game/weapons/NoWeapon.h>


Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        Character(position, velocity, 1000),
        is_alive_(true) {
    weapons_in_backpack_.push_back(
        std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("m4")));
    weapons_in_backpack_.push_back(
        std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("desert_eagle")));
    weapons_in_backpack_.push_back(
        std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("shotgun")));
    weapons_in_backpack_.push_back(
        std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("rocket_launcher")));
    weapons_in_backpack_.push_back(
        std::make_unique<NoWeapon>());
    weapons_in_backpack_.push_back(
        std::make_unique<NoWeapon>());

    current_weapon_ = weapons_in_backpack_.begin();
}

void Player::setDead() {
    is_alive_ = false;
}

bool Player::isAlive() const {
    return is_alive_;
}
