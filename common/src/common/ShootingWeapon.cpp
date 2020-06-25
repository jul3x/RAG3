//
// Created by jul3x on 16.10.19.
//

#include <utility>

#include <common/ResourceManager.h>

#include <common/ShootingWeapon.h>


ShootingWeapon::ShootingWeapon(Character* user, const std::string& id) :
        spawn_timeout_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "spawn_timeout")),
        recoil_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "recoil")),
        ammunition_(utils::j3x::get<int>(RM.getObjectParams("weapons", id), "max_ammo")),
        max_ammunition_(utils::j3x::get<int>(RM.getObjectParams("weapons", id), "max_ammo")),
        spawn_type_(utils::j3x::get<std::string>(RM.getObjectParams("weapons", id), "spawn_type")),
        spawn_quantity_(utils::j3x::get<int>(RM.getObjectParams("weapons", id), "spawn_quantity")),
        spawn_angular_diff_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "spawn_angular_diff")),
        spawn_offset_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "spawn_offset")),
        spawn_offset_factor_(-1),
        AbstractWeapon(user,
                       {utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")},
                       {utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")},
                       id)
{
    if (max_ammunition_ <= 0 || spawn_timeout_ <= 0.0f)
        throw std::invalid_argument("[ShootingWeapon] Constructor parameters are invalid!");

    this->changeOrigin(sf::Vector2f(0.0f,
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")));
}

sf::Vector2f ShootingWeapon::use()
{
    if (ammunition_ > 0 && time_elapsed_ < 0.0f)
    {
        auto sine = static_cast<float>(std::sin(this->getRotation() * M_PI / 180.0f));
        auto cosine = static_cast<float>(std::cos(this->getRotation() * M_PI / 180.0f));
        auto offset_position = this->getPosition();
        auto weapon_size = sf::Vector2f{this->getSize().x - weapon_offset_.x + BULLET_STARTING_OFFSET_,
                                        spawn_offset_factor_ * spawn_offset_};
        offset_position.x += weapon_size.x * cosine - 2.0f * weapon_size.y * sine;
        offset_position.y += weapon_size.x * sine + 2.0f * weapon_size.y * cosine;

        auto primary_rotation = this->getRotation() -
                                spawn_angular_diff_ * static_cast<float>(spawn_quantity_ - 1) / 2.0f;

        for (int i = 0; i < spawn_quantity_; ++i)
        {
            auto rotation = (primary_rotation + static_cast<float>(i) * spawn_angular_diff_) * M_PI / 180.0f;

            spawning_function_(user_, spawn_type_, offset_position, rotation);
        }

        time_elapsed_ = spawn_timeout_;

        --ammunition_;
        return -recoil_ * sf::Vector2f{cosine, sine};
    }

    return {0.0f, 0.0f};
}

int ShootingWeapon::getAmmunition() const
{
    return ammunition_;
}

float ShootingWeapon::getState() const
{
    return static_cast<float>(ammunition_) / static_cast<float>(max_ammunition_);
}

void ShootingWeapon::setState(float state)
{
    ammunition_ = static_cast<int>(state * max_ammunition_);
}

void ShootingWeapon::setFlipY(bool flip)
{
    AbstractDrawableObject::setFlipY(flip);
    spawn_offset_factor_ = flip ? 1 : -1;
}
