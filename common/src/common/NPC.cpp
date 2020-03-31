//
// Created by jul3x on 31.10.19.
//

#include <cmath>

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/ai/AStar.h>

#include <common/ResourceManager.h>

#include <common/NPC.h>

#include <common/NoWeapon.h>
#include <common/ShootingWeapon.h>


NPC::NPC(const sf::Vector2f& position, const std::string& id, int u_id) :
        Character(position, id, u_id),
        AbstractAgent(),
        visibility_state_(VisibilityState::TooFar)
{
    weapons_in_backpack_.push_back(std::make_shared<ShootingWeapon>("desert_eagle"));

    current_weapon_ = weapons_in_backpack_.begin();

    this->setPosition(position);
}


void NPC::registerEnemy(const Character* enemy)
{
    enemies_.push_back(enemy);
    current_enemy_ = enemy;
}

void NPC::removeEnemy(const Character* enemy)
{
    auto it = std::find(enemies_.begin(), enemies_.end(), enemy);

    if (it != enemies_.end())
    {
        enemies_.erase(it);
        current_enemy_ = enemies_.empty() ? nullptr : enemies_.front();
    }
}

void NPC::clearEnemies()
{
    enemies_.clear();
    current_enemy_ = nullptr;
}

void NPC::registerMapBlockage(const ai::MapBlockage* map_blockage)
{
    map_blockage_ = map_blockage;
}

bool NPC::update(float time_elapsed, float time_factor)
{
//    if (!this->isVisible()) return true;
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

const sf::Vector2f& NPC::getStartPosition() const
{
    return this->getPosition();
}

void NPC::handleEnemySelection()
{
    current_enemy_ = enemies_.front();
    auto current_distance = utils::geo::getDistance(current_enemy_->getPosition(), this->getPosition());
    for (const auto& enemy : enemies_)
    {
        auto new_distance = utils::geo::getDistance(enemy->getPosition(), this->getPosition());
        if (new_distance < current_distance)
        {
            current_enemy_ = enemy;
            current_distance = new_distance;
        }
    }

}

void NPC::handleVisibilityState()
{
    float start_x = std::round(this->getPosition().x / map_blockage_->scale_x_);
    float start_y = std::round(this->getPosition().y / map_blockage_->scale_y_);
    float goal_x = std::round(current_enemy_->getPosition().x / map_blockage_->scale_x_);
    float goal_y = std::round(current_enemy_->getPosition().y / map_blockage_->scale_y_);

    auto dir = utils::geo::getNormalized(current_enemy_->getPosition() - this->getPosition());

    float walls_between = 0;
    while (!utils::num::isNearlyEqual(start_x, goal_x, 1.0f) ||
           !utils::num::isNearlyEqual(start_y, goal_y, 1.0f))
    {
        start_x = start_x + dir.x;
        start_y = start_y + dir.y;

        auto rounded_x = static_cast<int>(std::round(start_x));
        auto rounded_y = static_cast<int>(std::round(start_y));

        if (rounded_x >= map_blockage_->blockage_.size() || rounded_x < 0 ||
            rounded_y >= map_blockage_->blockage_.at(0).size() || rounded_y < 0)
            break;

        walls_between += map_blockage_->blockage_.at(rounded_x).at(rounded_y);

        if (walls_between > NPC::WALLS_BETWEEN_FAR_) break;
    }

    if (utils::geo::getDistance(current_enemy_->getPosition(), this->getPosition()) > NPC::MAX_DISTANCE_)
        visibility_state_ = VisibilityState::OutOfRange;
    else if (walls_between <= NPC::WALLS_BETWEEN_CLOSE_)
        visibility_state_ = VisibilityState::Close;
    else if (walls_between <= NPC::WALLS_BETWEEN_FAR_)
        visibility_state_ = VisibilityState::Far;
    else
        visibility_state_ = VisibilityState::TooFar;
}

void NPC::handleActionState()
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

sf::Vector2f NPC::findNearestSafeSpot(const sf::Vector2f& direction) const
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