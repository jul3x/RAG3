//
// Created by jprolejko on 16.10.19.
//

#include <objects/Weapon.h>
#include <system/Engine.h>

Weapon::Weapon(float bullet_timeout,
               float recoil,
               int amunition,
               const sf::Vector2f &size,
               const sf::Vector2f &weapon_offset,
               const std::string &bullet_type,
               int bullet_quantity,
               float bullet_angular_diff,
               const std::string &texture_name) :
    weapon_offset_(weapon_offset),
    bullet_timeout_(bullet_timeout),
    recoil_(recoil),
    amunition_(amunition),
    bullet_type_(bullet_type),
    bullet_quantity_(bullet_quantity),
    bullet_angular_diff_(bullet_angular_diff),
    AbstractDrawableObject({}, size,
                           "weapon_" + texture_name) {
    shape_.setOrigin(size / 2.0f - weapon_offset_);
}

sf::Vector2f Weapon::shot() {
    auto time_now = std::chrono::system_clock::now();
    if (amunition_ > 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            time_now - last_bullet_time_).count() >= bullet_timeout_)
    {
        float sine = std::sin(this->getRotation() * M_PI / 180.0f);
        float cosine = std::cos(this->getRotation() * M_PI / 180.0f);
        auto offset_position = this->getPosition();
        auto weapon_size = weapon_offset_ + sf::Vector2f{this->getSize().x, 0.0f} / 2.0f;
        offset_position.x += weapon_size.x * cosine - weapon_size.y * sine;
        offset_position.y += weapon_size.x * sine + weapon_size.y * cosine;

        auto primary_rotation = this->getRotation() - bullet_angular_diff_ * (bullet_quantity_ - 1) / 2.0f;

        Engine::getInstance().spawnSmokeAnimation(offset_position, 2.0f);
        for (int i = 0; i < bullet_quantity_; ++i)
        {
            auto rotation = (primary_rotation + i * bullet_angular_diff_) * M_PI / 180.0f;
            Engine::getInstance().spawnBullet(bullet_type_, offset_position, rotation);
        }

        last_bullet_time_ = time_now;

        --amunition_;
        return -recoil_ * sf::Vector2f{cosine, sine};
    }
    return {0.0f, 0.0f};
}