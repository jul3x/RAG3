//
// Created by jul3x on 03.11.19.
//


#include <characters/Player.h>
#include <weapons/ShootingWeapon.h>
#include <weapons/NoWeapon.h>
#include <misc/ResourceManager.h>


Player::Player(const sf::Vector2f& position,
               const sf::Vector2f& velocity) :
        Character(position, velocity, CFG.getInt("characters/player_max_health")),
        is_alive_(true)
{
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(RM.getWeapon("m4")));
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(RM.getWeapon("desert_eagle")));
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(RM.getWeapon("shotgun")));
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(RM.getWeapon("rocket_launcher")));
    weapons_in_backpack_.push_back(
            std::make_unique<NoWeapon>());
    weapons_in_backpack_.push_back(
            std::make_unique<NoWeapon>());

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
