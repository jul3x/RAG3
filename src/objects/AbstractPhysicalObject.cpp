//
// Created by jprolejko on 30.09.19.
//

#include <utils/Numeric.h>
#include <objects/AbstractPhysicalObject.h>


AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f &position,
                                               const sf::Vector2f &velocity,
                                               const sf::Vector2f &size,
                                               const std::string &texture_name,
                                               const double acceleration,
                                               const bool is_static) :
        AbstractDrawableObject(position, size, texture_name),
        curr_v_(velocity), set_v_(velocity),
        acceleration_(acceleration),
        is_static_(is_static) {}

const bool AbstractPhysicalObject::isStatic() const {
    return is_static_;
}

const sf::Vector2f& AbstractPhysicalObject::getVelocity() const {
    return curr_v_;
}

void AbstractPhysicalObject::setVelocity(const sf::Vector2f &velocity) {
    set_v_ = velocity;
}

void AbstractPhysicalObject::setVelocity(const float x, const float y) {
    set_v_.x = x;
    set_v_.y = y;
}

void AbstractPhysicalObject::setForcedVelocity(const sf::Vector2f &velocity) {
    curr_v_ = velocity;
}

void AbstractPhysicalObject::update(float time_elapsed) {
    if (!this->isStatic())
    {
        last_position_ = this->getPosition();
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
}
