//
// Created by jprolejko on 30.09.19.
//

#include <system/Engine.h>

#include <objects/Player.h>


Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        AbstractPhysicalObject(position,
                               velocity,
                               {SIZE_X_, SIZE_Y_},
                               "player",
                               CFG.getFloat("player_max_acceleration"),
                               false) {
    last_bullet_time_ = std::chrono::system_clock::now();
}

void Player::shot() {
    auto time_now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            time_now - last_bullet_time_).count() >= CFG.getFloat("bullet_timeout"))
    {
        float sine = std::sin(this->getRotation() * M_PI / 180.0f);
        float cosine = std::cos(this->getRotation() * M_PI / 180.0f);
        auto offset_position = this->getPosition();
        offset_position.x += GUN_OFFSET_X_ * cosine;
        offset_position.y += GUN_OFFSET_X_ * sine;
        Engine::getInstance().spawnBullet(offset_position, this->getRotation() * M_PI / 180.0f);
        last_bullet_time_ = time_now;

        this->setForcedVelocity({-CFG.getFloat("recoil") * cosine, -CFG.getFloat("recoil") * sine});
        Engine::getInstance().forceCameraShaking();
    }
}