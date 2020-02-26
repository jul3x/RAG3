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
            std::make_unique<ShootingWeapon>(RM.getWeapon("desert_eagle")));

    current_weapon_ = weapons_in_backpack_.begin();

    this->setNoGoal();
}

bool Enemy::update(float time_elapsed)
{
    if (!this->isVisible()) return true;
    bool is_alive = Character::update(time_elapsed);

    auto& player_position = Game::get().getPlayerPosition();

    handleVisibilityState();
    handleActionState();

    auto velocity = CFG.getFloat("enemy_max_speed") * this->generateVelocityForPath();

    switch (action_state_)
    {
        case ActionState::StandBy:
        {
            velocity = CFG.getFloat("enemy_standby_speed") * this->getWanderingDirection(0.2f, 100.0f, 20);

            this->setNoGoal();
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::Follow:
        {
            this->setWeaponPointing(player_position);
            this->setCurrentGoal(player_position);
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::DestroyWall:
        {
            this->setWeaponPointing(player_position);
            this->setNoGoal();

            if (this->isAlreadyRotated())
                this->shot();
            break;
        }
        case ActionState::Shot:
        {
            this->setWeaponPointing(player_position);
            this->setNoGoal();

            if (this->isAlreadyRotated())
                this->shot();

            break;
        }
        case ActionState::ShotAndRun:
        {
            this->setWeaponPointing(player_position);
            this->setCurrentGoal(this->findNearestSafeSpot(this->getPosition() - player_position));

            if (this->isAlreadyRotated())
                this->shot();
            break;
        }
        case ActionState::Run:
        {
            this->setWeaponPointing(this->getPosition() + velocity);
            this->setCurrentGoal(this->findNearestSafeSpot(this->getPosition() - player_position));
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

void Enemy::handleVisibilityState()
{
    auto& blockage = Game::get().getMapBlockage();
    float start_x = std::round(this->getPosition().x / blockage.scale_x_);
    float start_y = std::round(this->getPosition().y / blockage.scale_y_);
    float goal_x = std::round(Game::get().getPlayerPosition().x / blockage.scale_x_);
    float goal_y = std::round(Game::get().getPlayerPosition().y / blockage.scale_y_);

    auto dir = utils::geo::getNormalized(Game::get().getPlayerPosition() - this->getPosition());

    int walls_between = 0;
    while (!utils::num::isNearlyEqual(start_x, goal_x, 1.0f) ||
           !utils::num::isNearlyEqual(start_y, goal_y, 1.0f))
    {
        start_x = start_x + dir.x;
        start_y = start_y + dir.y;

        auto rounded_x = static_cast<int>(std::round(start_x));
        auto rounded_y = static_cast<int>(std::round(start_y));

        if (rounded_x >= blockage.blockage_.size() || rounded_x < 0 ||
            rounded_y >= blockage.blockage_.at(0).size() || rounded_y < 0)
            break;

        if (blockage.blockage_.at(rounded_x).at(rounded_y))
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

sf::Vector2f Enemy::findNearestSafeSpot(const sf::Vector2f& direction) const
{
    auto dir = utils::geo::getNormalized(direction);
    auto& blockage = Game::get().getMapBlockage();
    auto current = sf::Vector2f{std::round(this->getPosition().x / blockage.scale_x_),
                                std::round(this->getPosition().y / blockage.scale_y_)};

    int rounded_x = static_cast<int>(std::round(current.x + dir.x));
    int rounded_y = static_cast<int>(std::round(current.y + dir.y));
    if (rounded_x < blockage.blockage_.size() && rounded_x >= 0 &&
        rounded_y < blockage.blockage_.at(0).size() && rounded_y >= 0 &&
        !blockage.blockage_.at(rounded_x).at(rounded_y))
        return {rounded_x * blockage.scale_x_, rounded_y * blockage.scale_y_};

    rounded_x = static_cast<int>(std::round(current.x - dir.y));
    rounded_y = static_cast<int>(std::round(current.y + dir.x));
    if (rounded_x < blockage.blockage_.size() && rounded_x >= 0 &&
        rounded_y < blockage.blockage_.at(0).size() && rounded_y >= 0 &&
        !blockage.blockage_.at(rounded_x).at(rounded_y))
        return {rounded_x * blockage.scale_x_, rounded_y * blockage.scale_y_};

    rounded_x = static_cast<int>(std::round(current.x + dir.y));
    rounded_y = static_cast<int>(std::round(current.y - dir.x));
    if (rounded_x < blockage.blockage_.size() && rounded_x >= 0 &&
        rounded_y < blockage.blockage_.at(0).size() && rounded_y >= 0 &&
        !blockage.blockage_.at(rounded_x).at(rounded_y))
        return {rounded_x * blockage.scale_x_, rounded_y * blockage.scale_y_};

    rounded_x = static_cast<int>(std::round(current.x - dir.x));
    rounded_y = static_cast<int>(std::round(current.y - dir.y));
    if (rounded_x < blockage.blockage_.size() && rounded_x >= 0 &&
        rounded_y < blockage.blockage_.at(0).size() && rounded_y >= 0 &&
        !blockage.blockage_.at(rounded_x).at(rounded_y))
        return {rounded_x * blockage.scale_x_, rounded_y * blockage.scale_y_};

    return {};
}