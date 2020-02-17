//
// Created by jul3x on 31.10.19.
//

#include <cmath>

#include <engine/utils/Geometry.h>
#include <engine/ai/AStar.h>

#include <game/misc/ResourceManager.h>
#include <game/characters/Enemy.h>
#include <game/Game.h>


Enemy::Enemy(const sf::Vector2f& position,
             const sf::Vector2f& velocity) :
        Character(position, velocity, 10),
        AbstractAgent(Game::get().getAgentsManager())
{
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("desert_eagle")));

    current_weapon_ = weapons_in_backpack_.begin();

    this->setNoGoal();
}

bool Enemy::update(float time_elapsed)
{
    handleLifeState();
    handleAmmoState();
    handleVisibilityState();
    handleActionState();

    bool is_alive = Character::update(time_elapsed);
    float rotation;
    std::tie(std::ignore, rotation) =
            utils::geo::cartesianToPolar(Game::get().getPlayerPosition() - this->getPosition());

    this->setRotation(rotation * 180.0f / static_cast<float>(M_PI));

    this->setCurrentGoal(Game::get().getPlayerPosition());

    //shot();

    path_ = &(this->getPath());
    this->setVelocity(CFG.getFloat("enemy_max_speed") * this->generateVelocityForPath());

    //std::cout << "Life: " << static_cast<int>(life_state_) << ", ammo: " << static_cast<int>(ammo_state_) << std::endl;

    return is_alive;
}

const sf::Vector2f& Enemy::getStartPosition() const
{
    return this->getPosition();
}

void Enemy::handleLifeState()
{
    if (life_ > 0.67 * max_life_)
        life_state_ = LifeState::High;
    else if (life_ > 0.2 * max_life_)
        life_state_ = LifeState::Low;
    else
        life_state_ = LifeState::Critical;
}

void Enemy::handleAmmoState()
{
    if ((*current_weapon_)->getState() > 0.5)
        ammo_state_ = AmmoState::High;
    else if ((*current_weapon_)->getState() > 0.0)
        ammo_state_ = AmmoState::Low;
    else
        ammo_state_ = AmmoState::Zero;
}

void Enemy::handleVisibilityState()
{
    // TODO
}

void Enemy::handleActionState()
{
    // TODO
}