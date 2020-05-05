//
// Created by jul3x on 03.11.19.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/NoWeapon.h>

#include <characters/Player.h>
#include <common/ShootingWeapon.h>


Player::Player(const sf::Vector2f& position) :
        Character(position, "player"),
        is_alive_(true),
        side_stepping_freeze_time_(-1.0f)
{
    if (CFG.get<int>("no_clip_mode"))
    {
        this->changeCollisionArea(Collision::None());
    }
}

void Player::setDead()
{
    is_alive_ = false;
}

bool Player::isAlive() const
{
    return is_alive_;
}

bool Player::sideStep(Player::SideStepDir dir)
{
    if (side_stepping_freeze_time_ <= 0.0f)
    {
        this->setForcedVelocity(this->getSpeedFactor() *
                                utils::geo::polarToCartesian(
                                        utils::j3x::get<float>(RM.getObjectParams("characters", "player"),
                                                               "side_step_speed"),
                                        (this->getRotation() + static_cast<int>(dir) * 90.0f) * M_PI / 180.0f));

        side_stepping_freeze_time_ = CFG.get<float>("side_stepping_freeze_time");
    }
}

bool Player::update(float time_elapsed)
{
    side_stepping_freeze_time_ -= time_elapsed;

    return Character::update(time_elapsed);
}

void Player::setHealth(float life)
{
    if (!CFG.get<int>("god_mode"))
    {
        Character::setHealth(life);
    }
}

void Player::getShot(const Bullet& bullet)
{
    if (!CFG.get<int>("god_mode"))
    {
        Character::getShot(bullet);
    }
}

void Player::handleGlobalState(float time_elapsed)
{
    switch (global_state_)
    {
        case GlobalState::Normal:
            break;

        case GlobalState::OnFire:
            if (!CFG.get<int>("god_mode"))
            {
                life_ -= time_elapsed * CFG.get<float>("on_fire_hurt_speed");
            }
            on_fire_time_ -= time_elapsed;

            if (on_fire_time_ <= 0.0f)
                setGlobalState(GlobalState::Normal);
            break;
    }
}
