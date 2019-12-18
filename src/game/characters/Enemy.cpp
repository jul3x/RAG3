//
// Created by jul3x on 31.10.19.
//

#include <game/misc/ResourceManager.h>
#include <engine/system/Engine.h>
#include <engine/utils/Geometry.h>
#include <game/characters/Enemy.h>


Enemy::Enemy(const sf::Vector2f& position,
             const sf::Vector2f& velocity) :
        Character(position, velocity, 10)
{
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("desert_eagle")));

    current_weapon_ = weapons_in_backpack_.begin();
}

bool Enemy::update(float time_elapsed)
{
    bool is_alive = Character::update(time_elapsed);
    float rotation;
    std::tie(std::ignore, rotation) =
            utils::cartesianToPolar(Engine::getInstance().getPlayer().getPosition() - this->getPosition());

    this->setRotation(rotation * 180.0f / static_cast<float>(M_PI));
    shot();

    return is_alive;
}
