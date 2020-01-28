//
// Created by jul3x on 31.10.19.
//

#include <game/misc/ResourceManager.h>
#include <engine/utils/Geometry.h>
#include <game/characters/Enemy.h>
#include <game/Game.h>
#include <game/weapons/NoWeapon.h>


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
            utils::geo::cartesianToPolar(Game::get().getPlayerPosition() - this->getPosition());
    
    this->setRotation(rotation * 180.0f / static_cast<float>(M_PI));

    int noise = utils::num::getRandom(-10, 10);
    this->setVelocity(3 * std::cos(rotation) - noise * std::sin(rotation), 3 * std::sin(rotation) + noise * std::cos(rotation));
    shot();

    return is_alive;
}
