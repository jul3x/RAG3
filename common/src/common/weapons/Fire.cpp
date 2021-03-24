//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/weapons/Fire.h>
#include <common/ResourceManager.h>



Fire::Fire(Character* user,
           const sf::Vector2f& position,
           const float direction) :
        HoveringObject(position, CONF<float>("fire_speed") * sf::Vector2f(std::cos(direction), std::sin(direction)),
                       CONF<float>("graphics/fire_image_size") * sf::Vector2f{1.0f, 1.0f},
                       collision::Circle(CONF<float>("fire_initial_radius") *
                               CONF<float>("fire_collision_factor"),
                                       {0.0F, CONF<float>("fire_collision_offset")}),
                       &RM.getTexture("fire"),
                       CONF<float>("fire_z_index"),
                       CONF<int>("graphics/fire_frames"),
                       CONF<float>("fire_life") / CONF<int>("graphics/fire_frames"), 0.0f),
        user_(user),
        direction_(direction),
        color_(sf::Color(CONF<int>("graphics/fire_color"))),
        alpha_(0.0f),
        life_(CONF<float>("fire_life")),
        r_(CONF<float>("fire_initial_radius")),
        offset_(utils::num::getRandom(0.0, 2 * M_PI))
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI) + 90.0f);
    this->changeOrigin({r_, r_/2.0f});

    this->makeLightPoint(this->getPosition(),
                         CONF<float>("graphics/fire_light_point_size") * CONF<float>("graphics/global_zoom"),
                         &RM.getTexture("lightpoint"), "const");
    light_->setColor(255, 255, 255, alpha_);

    this->setSize({r_ * 2.0f, r_ * 2.0f});
    this->setColor(color_.r, color_.g, color_.b, static_cast<sf::Uint8>(alpha_));
}

void Fire::setDead()
{
    life_ = -1.0f;
}

bool Fire::update(float time_elapsed)
{
    this->updateAnimation(time_elapsed, CONF<float>("graphics/fire_frame_times"));
    DynamicObject::update(time_elapsed);

    difference_ = CONF<float>("fire_spread_distance") *
            std::cos(offset_ + life_ / CONF<float>("fire_life") * 4 * M_PI);
    this->setForcedVelocity(this->getVelocity() +
                      difference_ * time_elapsed * sf::Vector2f{static_cast<float>(std::cos(direction_ + M_PI_2)),
                                                 static_cast<float>(std::sin(direction_ + M_PI_2))});

    r_ += time_elapsed * CONF<float>("fire_spread_speed");
    alpha_ = - life_ * life_ * 4 * 255.0f / (CONF<float>("fire_life") * CONF<float>("fire_life")) +
            4 * 255.0f / CONF<float>("fire_life") * life_; // ax^2 + bx + c

    light_->setPosition(this->getPosition());
    light_->setColor(255, 255, 255, alpha_);

    this->setSize({r_ * 2.0f, r_ * 2.0f});
    this->setColor(color_.r, color_.g, color_.b, static_cast<sf::Uint8>(alpha_));
    this->changeCollisionArea(collision::Circle(r_ * CONF<float>("fire_collision_factor"),
                                                {0.0F, CONF<float>("fire_collision_offset")}));

    life_ -= time_elapsed;

    return life_ > 0.0f;
}

Character* Fire::getUser() const
{
    return user_;
}

float Fire::getAlpha() const
{
    return alpha_;
}

float Fire::getR() const
{
    return r_;
}

void Fire::setAlpha(float alpha)
{
    alpha_ = alpha;
    this->setColor(color_.r, color_.g, color_.b, static_cast<sf::Uint8>(alpha_));
}

void Fire::setR(float r)
{
    r_ = r;
    this->setSize({r_ * 2.0f, r_ * 2.0f});
}

void Fire::setSize(const sf::Vector2f& size)
{
    shape_.setSize(size);
}
