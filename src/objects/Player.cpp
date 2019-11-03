//
// Created by jprolejko on 03.11.19.
//

#include <system/ResourceManager.h>

#include <objects/Player.h>


Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        Character(position, velocity) {
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("m4"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("desert_eagle"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("shotgun"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("rocket_launcher"));

    current_weapon_ = weapons_in_backpack_.begin();
}
