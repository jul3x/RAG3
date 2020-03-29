//
// Created by jul3x on 27.03.20.
//

#include <common/ResourceManager.h>
#include <common/ShootingWeapon.h>

#include <characters/PlayerClone.h>


PlayerClone::PlayerClone(const sf::Vector2f& position, const Player* player, float life_time) :
        NPC(position, "player"),
        player_(player),
        life_time_(life_time)
{
}

void PlayerClone::updateLifeTimeDependingOnPrevious(float life_time)
{
    life_time_ -= life_time;
}

bool PlayerClone::isLifeTime() const
{
    return life_time_ > 0.0f;
}

bool PlayerClone::update(float time_elapsed, float time_factor)
{
    life_time_ -= time_elapsed;

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
            this->setCurrentGoal(player_->getPosition());

            if (velocity != sf::Vector2f{})
                this->setWeaponPointing(this->getPosition() + velocity);
            break;
        }
        case ActionState::Follow:
        {
            this->setWeaponPointing(enemy_position);
            this->setCurrentGoal(enemy_position);

            if (velocity != sf::Vector2f{})
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
