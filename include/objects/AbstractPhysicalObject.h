//
// Created by jprolejko on 16.06.19.
//


#ifndef RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H
#define RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H

#include <iostream>
#include <cmath>

#include <utils/Numeric.h>
#include <objects/AbstractDrawableObject.h>


class AbstractPhysicalObject : public AbstractDrawableObject {
public:
    AbstractPhysicalObject() = default;

    AbstractPhysicalObject(const sf::Vector2f &position,
                           const sf::Vector2f &velocity,
                           const sf::Vector2f &size,
                           const std::string &texture_name,
                           const double acceleration,
                           const bool is_static = true) :
        AbstractDrawableObject(position, size, texture_name),
        curr_v_(velocity), set_v_(velocity),
        acceleration_(acceleration),
        is_static_(is_static) {}

    const bool isStatic() const {
        return is_static_;
    }

    const sf::Vector2f& getVelocity() const {
        return curr_v_;
    }

    void setVelocity(const sf::Vector2f &velocity) {
        set_v_ = velocity;
    }

    void setVelocity(const float x, const float y) {
        set_v_.x = x;
        set_v_.y = y;
    }

    void setForcedVelocity(const sf::Vector2f &velocity) {
        curr_v_ = velocity;
    }

    void update(float time_elapsed) {
        if (!this->isStatic())
        {
            if (utils::isNearlyEqual(set_v_.x, 0.0f, 0.01f) &&
                utils::isNearlyEqual(curr_v_.x, 0.0f, 0.1f))
            {
                curr_v_.x = 0.0f;
            }
            else
            {
                curr_v_.x = curr_v_.x - std::copysign(acceleration_ * time_elapsed, curr_v_.x - set_v_.x);
            }
            if (utils::isNearlyEqual(set_v_.y, 0.0f, 0.01f) &&
                utils::isNearlyEqual(curr_v_.y, 0.0f, 0.1f))
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

private:
    float acceleration_;

    sf::Vector2f curr_v_, set_v_;
    bool is_static_;

};

#endif // RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H
