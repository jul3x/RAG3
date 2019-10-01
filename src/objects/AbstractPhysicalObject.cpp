//
// Created by jprolejko on 30.09.19.
//

#include <utils/Numeric.h>
#include <objects/AbstractPhysicalObject.h>

//
// AbstractPhysicalObject
//

AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f &position,
                                               const sf::Vector2f &size,
                                               const std::string &texture_name) :
        AbstractDrawableObject(position, size, texture_name) {}

//
// StaticObject
//

StaticObject::StaticObject(const sf::Vector2f &position,
                           const sf::Vector2f &size,
                           const std::string &texture_name) :
        AbstractPhysicalObject(position, size, texture_name) {}

void StaticObject::update(float time_elapsed) {}

//
// DynamicObject
//

DynamicObject::DynamicObject(const sf::Vector2f &position,
                             const sf::Vector2f &velocity,
                             const sf::Vector2f &size,
                             const std::string &texture_name,
                             const double acceleration) :
        StaticObject(position, size, texture_name),
        curr_v_(velocity), set_v_(velocity),
        acceleration_(acceleration) {}

const sf::Vector2f& DynamicObject::getVelocity() const {
    return curr_v_;
}

void DynamicObject::setVelocity(const sf::Vector2f &velocity) {
    set_v_ = velocity;
}

void DynamicObject::setVelocity(const float x, const float y) {
    set_v_.x = x;
    set_v_.y = y;
}

void DynamicObject::setForcedVelocity(const sf::Vector2f &velocity) {
    curr_v_ = velocity;
}

void DynamicObject::update(float time_elapsed) {
    if (utils::isNearlyEqual(set_v_.x, 0.0f, 0.01f) &&
        utils::isNearlyEqual(curr_v_.x, 0.0f, 0.05f))
    {
        curr_v_.x = 0.0f;
    }
    else
    {
        curr_v_.x = curr_v_.x - std::copysign(acceleration_ * time_elapsed, curr_v_.x - set_v_.x);
    }
    if (utils::isNearlyEqual(set_v_.y, 0.0f, 0.01f) &&
        utils::isNearlyEqual(curr_v_.y, 0.0f, 0.05f))
    {
        curr_v_.y = 0.0f;
    }
    else
    {
        curr_v_.y = curr_v_.y - std::copysign(acceleration_ * time_elapsed, curr_v_.y - set_v_.y);
    }

    this->setPosition(this->getPosition() + curr_v_);
}

//
// DynamicObject
//

HoveringObject::HoveringObject(const sf::Vector2f &position,
                               const sf::Vector2f &velocity,
                               const sf::Vector2f &size,
                               const std::string &texture_name,
                               const double acceleration) :
        DynamicObject(position, velocity, size, texture_name, acceleration) {}
