//
// Created by jul3x on 27.03.20.
//

#include <common/ResourceManager.h>
#include <common/ShootingWeapon.h>

#include <characters/PlayerClone.h>


PlayerClone::PlayerClone(const sf::Vector2f& position) :
        NPC(position, "player")
{
}

bool PlayerClone::update(float time_elapsed, float time_factor)
{
    if (!this->isVisible()) return true;
    bool is_alive = Character::update(time_elapsed);

    handleEnemySelection();
    handleVisibilityState();
    handleActionState();

    auto& enemy_position = current_enemy_->getPosition();
    auto velocity = utils::getFloat(RM.getObjectParams("characters", this->getId()), "max_speed") * this->generateVelocityForPath();

    switch (action_state_)
    {
        case ActionState::StandBy:
        {
            velocity = utils::getFloat(RM.getObjectParams("characters", this->getId()), "standby_speed") * this->getWanderingDirection(0.2f, 100.0f, 20);

            this->setNoGoal();
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::Follow:
        {
            this->setWeaponPointing(enemy_position);
            this->setCurrentGoal(enemy_position);
            this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::DestroyWall:
        {
            this->setWeaponPointing(enemy_position);
            this->setNoGoal();

            if (this->isAlreadyRotated())
                this->shot(time_factor);
            break;
        }
        case ActionState::Shot:
        {
            this->setWeaponPointing(enemy_position);
            this->setNoGoal();

            if (this->isAlreadyRotated())
                this->shot(time_factor);

            break;
        }
        case ActionState::ShotAndRun:
        {
            this->setWeaponPointing(enemy_position);
            this->setCurrentGoal(this->findNearestSafeSpot(this->getPosition() - enemy_position));

            if (this->isAlreadyRotated())
                this->shot(time_factor);
            break;
        }
        case ActionState::Run:
        {
            this->setWeaponPointing(this->getPosition() + velocity);
            this->setCurrentGoal(this->findNearestSafeSpot(this->getPosition() - enemy_position));
            break;
        }
    }

    this->setVelocity(velocity);

    return is_alive;
}

void PlayerClone::handleActionState()
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

sf::Vector2f PlayerClone::findNearestSafeSpot(const sf::Vector2f& direction) const
{
    auto dir = utils::geo::getNormalized(direction);
    auto current = sf::Vector2f{std::round(this->getPosition().x / map_blockage_->scale_x_),
                                std::round(this->getPosition().y / map_blockage_->scale_y_)};

    auto checkIfPositionValid = [this](int a, int b) {
        return a < this->map_blockage_->blockage_.size() && a >= 0 &&
               b < this->map_blockage_->blockage_.at(0).size() && b >= 0 &&
               !this->map_blockage_->blockage_.at(a).at(b);
    };

    int rounded_x = static_cast<int>(std::round(current.x + dir.x));
    int rounded_y = static_cast<int>(std::round(current.y + dir.y));
    if (checkIfPositionValid(rounded_x, rounded_y))
        return {rounded_x * map_blockage_->scale_x_, rounded_y * map_blockage_->scale_y_};

    rounded_x = static_cast<int>(std::round(current.x - dir.y));
    rounded_y = static_cast<int>(std::round(current.y + dir.x));
    if (checkIfPositionValid(rounded_x, rounded_y))
        return {rounded_x * map_blockage_->scale_x_, rounded_y * map_blockage_->scale_y_};

    rounded_x = static_cast<int>(std::round(current.x + dir.y));
    rounded_y = static_cast<int>(std::round(current.y - dir.x));
    if (checkIfPositionValid(rounded_x, rounded_y))
        return {rounded_x * map_blockage_->scale_x_, rounded_y * map_blockage_->scale_y_};

    rounded_x = static_cast<int>(std::round(current.x - dir.x));
    rounded_y = static_cast<int>(std::round(current.y - dir.y));
    if (checkIfPositionValid(rounded_x, rounded_y))
        return {rounded_x * map_blockage_->scale_x_, rounded_y * map_blockage_->scale_y_};

    return {ai::NO_GOAL, ai::NO_GOAL};
}