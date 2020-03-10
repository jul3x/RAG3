//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>
#include <weapons/Bullet.h>
#include <misc/ResourceManager.h>
#include <Game.h>


Bullet::Bullet(const BulletDescription& description,
               const sf::Vector2f& position,
               const float direction) :
        HoveringObject(position,
                       {description.speed_ * std::cos(direction),
                        description.speed_ * std::sin(direction)},
                       {description.size_x_, description.size_y_},
                       Collision::Box(description.size_x_, description.size_y_),
                       &RM.getTexture("bullet_" + description.name_),
                       sf::Color(CFG.getInt("trail_color")),
                       0.0f)
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI));
    life_ = description.life_;
    deadly_factor_ = description.deadly_factor_;
}

int Bullet::getDeadlyFactor() const
{
    return deadly_factor_;
}

void Bullet::setDead()
{
    life_ = -1.0f;
}

bool Bullet::update(float time_elapsed)
{
    DynamicObject::update(time_elapsed);

    life_ -= time_elapsed * 1000.0f;

    return life_ > 0.0f;
}