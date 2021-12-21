//
// Created by jul3x on 05.05.2020.
//

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/misc/Explosion.h>


Explosion::Explosion(const sf::Vector2f& position, float r) :
        HoveringObject(position, {}, {2 * r, 2 * r},
                       collision::Circle(r), nullptr, 0, 1, 1, 0.0f),
        r_(r)
{

}

bool Explosion::update(float time_elapsed)
{
    return false;
}

void Explosion::applyForce(Character* obj, float factor_) const
{
    auto vector = utils::geo::cartesianToPolar(obj->getPosition() - this->getPosition());
    auto factor = factor_ * r_ / std::max(std::get<0>(vector), 32.0f);
    auto& theta = std::get<1>(vector);
    obj->setHealth(obj->getHealth() - CONF<float>("explosion_hurt_factor") * factor);

    obj->addSteeringForce(CONF<float>("explosion_velocity_factor") * factor *
                          sf::Vector2f{std::cos(theta), std::sin(theta)}, CONF<float>("explosion_force_duration"));
}

void Explosion::applyForce(Obstacle* obj) const
{
    auto distance = utils::geo::getDistance(obj->getPosition(), this->getPosition());
    obj->setHealth(obj->getHealth() - CONF<float>("explosion_hurt_factor") * r_ / distance);
}
