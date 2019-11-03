//
// Created by jprolejko on 31.10.19.
//

#include <system/ResourceManager.h>
#include <system/Engine.h>
#include <utils/Geometry.h>
#include <objects/Enemy.h>


Enemy::Enemy(const sf::Vector2f &position,
             const sf::Vector2f &velocity) :
        Character(position,
                  velocity) {
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("desert_eagle"));

    current_weapon_ = weapons_in_backpack_.begin();
}

bool Enemy::update(float time_elapsed) {
    bool is_alive = Character::update(time_elapsed);
    float rotation;
    std::tie(std::ignore, rotation) =
        utils::cartesianToPolar(Engine::getInstance().getPlayer().getPosition() - this->getPosition());

    this->setRotation(rotation * 180.0f / M_PI);
    shot();

    return is_alive;
}
