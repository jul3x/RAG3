//
// Created by jul3x on 03.11.19.
//

#include <common/ResourceManager.h>
#include <common/NoWeapon.h>

#include <characters/Player.h>
#include <common/ShootingWeapon.h>


Player::Player(const sf::Vector2f& position) :
        Character(position, "player"),
        is_alive_(true)
{
    weapons_in_backpack_.push_back(
            std::make_shared<ShootingWeapon>("m4"));
    weapons_in_backpack_.push_back(
            std::make_shared<ShootingWeapon>("desert_eagle"));
    weapons_in_backpack_.push_back(
            std::make_shared<ShootingWeapon>("shotgun"));
    weapons_in_backpack_.push_back(
            std::make_shared<ShootingWeapon>("rocket_launcher"));
    weapons_in_backpack_.push_back(
            std::make_shared<NoWeapon>());

    current_weapon_ = weapons_in_backpack_.begin();
}

void Player::setDead()
{
    is_alive_ = false;
}

bool Player::isAlive() const
{
    return is_alive_;
}
