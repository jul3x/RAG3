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
    weapons_in_backpack_.push_back(std::make_shared<ShootingWeapon>("m4"));
    weapons_in_backpack_.push_back(std::make_shared<NoWeapon>());
    weapons_in_backpack_.push_back(std::make_shared<NoWeapon>());
    weapons_in_backpack_.push_back(std::make_shared<NoWeapon>());

    current_weapon_ = weapons_in_backpack_.begin();
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
        this->setForcedVelocity(
                utils::geo::polarToCartesian(
                        utils::getFloat(RM.getObjectParams("characters", "player"), "side_step_speed"),
                        (this->getRotation() + static_cast<int>(dir) * 90.0f) * M_PI / 180.0f));

        side_stepping_freeze_time_ = CFG.getFloat("side_stepping_freeze_time");
    }
}

bool Player::update(float time_elapsed)
{
    side_stepping_freeze_time_ -= time_elapsed;
    return Character::update(time_elapsed);
}

