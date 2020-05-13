//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/Fire.h>
#include <common/ResourceManager.h>



Fire::Fire(Character* user,
           const sf::Vector2f& position,
           const float direction) :
        HoveringObject(position, CFG.get<float>("fire_speed") * sf::Vector2f(std::cos(direction), std::sin(direction)),
                       CFG.get<float>("graphics/fire_image_size") * sf::Vector2f{1.0f, 1.0f},
                       collision::Circle(CFG.get<float>("fire_initial_radius") *
                               CFG.get<float>("fire_collision_factor"),
                                       {0.0F, CFG.get<float>("fire_collision_offset")}),
                       &RM.getTexture("fire"),
                       CFG.get<float>("fire_z_index"),
                       CFG.get<int>("graphics/fire_frames"),
                       CFG.get<float>("fire_life") / CFG.get<int>("graphics/fire_frames"), 0.0f),
        user_(user),
        direction_(direction),
        color_(sf::Color(CFG.get<int>("graphics/fire_color"))),
        alpha_(255.0f),
        life_(CFG.get<float>("fire_life")),
        r_(CFG.get<float>("fire_initial_radius")),
        offset_(utils::num::getRandom(0.0, 2 * M_PI))
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI));
    this->changeOrigin({r_/2.0f, r_});
}

void Fire::setDead()
{
    life_ = -1.0f;
}

bool Fire::update(float time_elapsed)
{
    this->updateAnimation(time_elapsed);
    DynamicObject::update(time_elapsed);

    difference_ = CFG.get<float>("fire_spread_distance") *
            std::cos(offset_ + life_ / CFG.get<float>("fire_life") * 4 * M_PI);
    this->setForcedVelocity(this->getVelocity() +
                      difference_ * sf::Vector2f{static_cast<float>(std::cos(direction_ + M_PI_2)),
                                                 static_cast<float>(std::sin(direction_ + M_PI_2))});

    r_ += time_elapsed * CFG.get<float>("fire_spread_speed");
    alpha_ = std::sqrt(life_) * 255.0f / std::sqrt(CFG.get<float>("fire_life"));

    this->setSize({r_ * 2.0f, r_ * 2.0f});
    this->setColor(color_.r, color_.g, color_.b, static_cast<sf::Uint8>(alpha_));
    this->changeCollisionArea(collision::Circle(r_ * CFG.get<float>("fire_collision_factor"),
                                                {0.0F, CFG.get<float>("fire_collision_offset")}));

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
