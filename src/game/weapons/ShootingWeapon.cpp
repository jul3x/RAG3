//
// Created by jul3x on 16.10.19.
//

#include <game/weapons/ShootingWeapon.h>

#include <utility>
#include <game/Game.h>


ShootingWeapon::ShootingWeapon(float bullet_timeout,
                               float recoil,
                               int ammunition,
                               const sf::Vector2f& size,
                               const sf::Vector2f& weapon_offset,
                               std::string bullet_type,
                               int bullet_quantity,
                               float bullet_angular_diff,
                               const std::string& texture_name) :
        bullet_timeout_(bullet_timeout),
        recoil_(recoil),
        ammunition_(ammunition),
        max_ammunition_(ammunition),
        bullet_type_(std::move(bullet_type)),
        bullet_quantity_(bullet_quantity),
        bullet_angular_diff_(bullet_angular_diff),
        AbstractWeapon(size, weapon_offset, texture_name)
{
    if (ammunition <= 0 || bullet_timeout <= 0.0f)
        throw std::invalid_argument("[ShootingWeapon] Constructor parameters are invalid!");

    shape_.setOrigin(size / 2.0f - weapon_offset_);
}

sf::Vector2f ShootingWeapon::use()
{
    auto time_now = std::chrono::system_clock::now();
    if (ammunition_ > 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
                time_now - last_bullet_time_).count() >= bullet_timeout_)
    {
        auto sine = static_cast<float>(std::sin(this->getRotation() * M_PI / 180.0f));
        auto cosine = static_cast<float>(std::cos(this->getRotation() * M_PI / 180.0f));
        auto offset_position = this->getPosition();
        auto weapon_size = weapon_offset_ + sf::Vector2f{this->getSize().x, 0.0f} / 2.0f;
        offset_position.x += weapon_size.x * cosine - weapon_size.y * sine;
        offset_position.y += weapon_size.x * sine + weapon_size.y * cosine;

        auto primary_rotation = this->getRotation() -
                                bullet_angular_diff_ * static_cast<float>(bullet_quantity_ - 1) / 2.0f;

        Game::get().spawnShotAnimation(offset_position, this->getRotation(), std::sqrt(recoil_) / 2.0f);
        for (int i = 0; i < bullet_quantity_; ++i)
        {
            auto rotation = (primary_rotation + static_cast<float>(i) * bullet_angular_diff_) * M_PI / 180.0f;
            Game::get().spawnBullet(bullet_type_, offset_position, rotation);
        }

        last_bullet_time_ = time_now;

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