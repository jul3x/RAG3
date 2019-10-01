//
// Created by jprolejko on 01.10.19.
//

#include <system/Config.h>
#include <objects/Bullet.h>


Bullet::Bullet(const sf::Vector2f &position,
               const float direction) :
        HoveringObject(position,
                       {CFG.getFloat("bullet_speed") * std::cos(direction),
                        CFG.getFloat("bullet_speed") * std::sin(direction)},
                       {SIZE_X_, SIZE_Y_},
                       "bullet",
                       100.0f) {
    this->setRotation(direction * 180.0f / M_PI);
}
