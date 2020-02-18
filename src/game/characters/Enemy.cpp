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
        Character(position, velocity, CFG.getInt("enemy_max_health")),
        AbstractAgent(Game::get().getAgentsManager())
{
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("desert_eagle")));

    current_weapon_ = weapons_in_backpack_.begin();

    this->setNoGoal();
}

bool Enemy::update(float time_elapsed)
{
    if (!this->isVisible()) return true;
    bool is_alive = Character::update(time_elapsed);

    handleLifeState();
    handleAmmoState();
    handleVisibilityState();
    //handleActionState();
    action_state_ = ActionState::StandBy;
    auto velocity = CFG.getFloat("enemy_max_speed") * this->generateVelocityForPath();

    switch (action_state_)
    {
        case ActionState::StandBy:
        {
            velocity = CFG.getFloat("enemy_standby_speed") * this->getWanderingDirection(0.2f, 100.0f);

            this->setNoGoal();
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::Follow:
        {
            this->setWeaponPointing(Game::get().getPlayerPosition());
            this->setCurrentGoal(Game::get().getPlayerPosition());
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::DestroyWall:
        {
            this->setWeaponPointing(Game::get().getPlayerPosition());
            this->setNoGoal();
            this->shot();
            //std::cout << "DESTROYWALL" << std::endl;
            break;
        }
        case ActionState::Shot:
        {
            this->setWeaponPointing(Game::get().getPlayerPosition());
            this->setNoGoal();
            this->shot();
            //std::cout << "SHOT" << std::endl;
            break;
        }
        case ActionState::ShotAndRun:
        {
            this->setWeaponPointing(Game::get().getPlayerPosition());
            this->setNoGoal();
            this->shot();
            //std::cout << "SHOTANDRUN" << std::endl;
            break;
        }
        case ActionState::Run:
        {
            this->setWeaponPointing(this->getPosition() + velocity);
            this->setNoGoal();
            //std::cout << "RUN" << std::endl;
            break;
        }
    }

    path_ = &(this->getPath());
    this->setVelocity(velocity);

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
    if ((*current_weapon_)->getState() > 0.7)
        ammo_state_ = AmmoState::High;
    else if ((*current_weapon_)->getState() > 0.0)
        ammo_state_ = AmmoState::Low;
    else
        ammo_state_ = AmmoState::Zero;
}

void Enemy::handleVisibilityState()
{
    auto& blockage = Game::get().getMapBlockage();
    float start_x = std::round(this->getPosition().x / blockage.scale_x_);
    float start_y = std::round(this->getPosition().y / blockage.scale_y_);
    float goal_x = std::round(Game::get().getPlayerPosition().x / blockage.scale_x_);
    float goal_y = std::round(Game::get().getPlayerPosition().y / blockage.scale_y_);

    auto dir = utils::geo::getNormalized({goal_x - start_x, goal_y - start_y});

    int walls_between = 0;
    while (!utils::num::isNearlyEqual(start_x, goal_x, 1.0f) ||
           !utils::num::isNearlyEqual(start_y, goal_y, 1.0f))
    {
        start_x = start_x + dir.x;
        start_y = start_y + dir.y;

        if (blockage.blockage_.at(std::round(start_x)).at(std::round(start_y)))
            ++walls_between;
    }

    if (utils::geo::getDistance(Game::get().getPlayerPosition(), this->getPosition()) > Enemy::MAX_DISTANCE_)
        visibility_state_ = VisibilityState::OutOfRange;
    else if (walls_between <= 0)
        visibility_state_ = VisibilityState::Close;
    else if (walls_between <= 1)
        visibility_state_ = VisibilityState::Far;
    else
        visibility_state_ = VisibilityState::TooFar;
}

void Enemy::handleActionState()
{
    switch (visibility_state_)
    {
        case VisibilityState::Close:
        {
            switch (life_state_)
            {
                case LifeState::High:
                {
                    switch (ammo_state_)
                    {
                        case AmmoState::High:
                        case AmmoState::Low:
                        {
                            action_state_ = ActionState::Shot;
                            break;
                        }
                        case AmmoState::Zero:
                        {
                            action_state_ = ActionState::Run;
                            break;
                        }
                    }
                    break;
                }
                case LifeState::Low:
                {
                    switch (ammo_state_)
                    {
                        case AmmoState::High:
                        case AmmoState::Low:
                        {
                            action_state_ = ActionState::ShotAndRun;
                            break;
                        }
                        case AmmoState::Zero:
                        {
                            action_state_ = ActionState::Run;
                            break;
                        }
                    }
                    break;
                }
                case LifeState::Critical:
                {
                    action_state_ = ActionState::Run;
                    break;
                }
            }
            break;
        }
        case VisibilityState::Far:
        {
            switch (life_state_)
            {
                case LifeState::High:
                case LifeState::Low:
                {
                    switch (ammo_state_)
                    {
                        case AmmoState::High:
                        {
                            action_state_ = ActionState::DestroyWall;
                            break;
                        }
                        case AmmoState::Low:
                        {
                            action_state_ = ActionState::Follow;
                            break;
                        }
                        case AmmoState::Zero:
                        {
                            action_state_ = ActionState::Run;
                            break;
                        }
                    }
                    break;
                }
                case LifeState::Critical:
                {
                    action_state_ = ActionState::Run;
                    break;
                }
            }
            break;
        }
        case VisibilityState::TooFar:
        {
            switch (life_state_)
            {
                case LifeState::High:
                case LifeState::Low:
                {
                    switch (ammo_state_)
                    {
                        case AmmoState::High:
                        case AmmoState::Low:
                        {
                            action_state_ = ActionState::Follow;
                            break;
                        }
                        case AmmoState::Zero:
                        {
                            action_state_ = ActionState::Run;
                            break;
                        }
                    }
                    break;
                }
                case LifeState::Critical:
                {
                    action_state_ = ActionState::Run;
                    break;
                }
            }
            break;
        }
        case VisibilityState::OutOfRange:
        {
            action_state_ = ActionState::StandBy;
            break;
        }
    }
}