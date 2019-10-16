//
// Created by jprolejko on 16.10.19.
//

#include <objects/Weapon.h>
#include <system/Engine.h>

Weapon::Weapon(float bullet_timeout,
               float recoil,
               int amunition,
               const sf::Vector2f &size,
               const std::string &texture_name) :
    bullet_timeout_(bullet_timeout),
    recoil_(recoil),
    amunition_(amunition),
    AbstractDrawableObject({}, size,
                           "weapon_" + texture_name) {}

sf::Vector2f Weapon::shot(const sf::Vector2f &position,
                          const float angle,
                          const sf::Vector2f &offset) {
    auto time_now = std::chrono::system_clock::now();
    if (amunition_ > 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            time_now - last_bullet_time_).count() >= bullet_timeout_)
    {
        float sine = std::sin(angle * M_PI / 180.0f);
        float cosine = std::cos(angle * M_PI / 180.0f);
        auto offset_position = position;
        offset_position.x += offset.x * cosine - offset.y * sine;
        offset_position.y += offset.x * sine + offset.y * cosine;

        Engine::getInstance().spawnBullet(offset_position, angle * M_PI / 180.0f, 1);
        last_bullet_time_ = time_now;

        --amunition_;
        return -recoil_ * sf::Vector2f{cosine, sine};
    }
    return {0.0f, 0.0f};
}