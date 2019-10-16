//
// Created by jprolejko on 01.10.19.
//

#include <system/Config.h>
#include <objects/Bullet.h>


Bullet::Bullet(const sf::Vector2f &position,
               const float direction,
               const float bullet_life,
               const int deadly_factor) :
        HoveringObject(position,
                       {CFG.getFloat("bullet_speed") * std::cos(direction),
                        CFG.getFloat("bullet_speed") * std::sin(direction)},
                       {SIZE_X_, SIZE_Y_},
                       "bullet",
                       100.0f) {
    this->setRotation(direction * 180.0f / M_PI);
    life_ = bullet_life;
    spawn_time_ = std::chrono::system_clock::now();
    deadly_factor_ = deadly_factor;
}

const int Bullet::getDeadlyFactor() const {
    return deadly_factor_;
}

bool Bullet::updateBullet(float time_elapsed) {
    DynamicObject::update(time_elapsed);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - spawn_time_).count() > life_)
    {
        return false;
    }

    return true;
}