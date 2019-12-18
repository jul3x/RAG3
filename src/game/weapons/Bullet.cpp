//
// Created by jul3x on 01.10.19.
//

#include <engine/system/Config.h>
#include <game/weapons/Bullet.h>
#include <engine/system/AbstractResourceManager.h>


Bullet::Bullet(const BulletDescription& description,
               const sf::Vector2f& position,
               const float direction) :
        HoveringObject(position,
                       {description.speed_ * std::cos(direction),
                        description.speed_ * std::sin(direction)},
                       {description.size_x_, description.size_y_},
                       &AbstractResourceManager::getInstance().getTexture("bullet_" + description.name_),
                       0.0f)
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI));
    life_ = description.life_;
    spawn_time_ = std::chrono::system_clock::now();
    deadly_factor_ = description.deadly_factor_;
}

int Bullet::getDeadlyFactor() const
{
    return deadly_factor_;
}

bool Bullet::update(float time_elapsed)
{
    DynamicObject::update(time_elapsed);

    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - spawn_time_).count() <= life_;
}