//
// Created by jul3x on 30.09.19.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/system/Config.h>
#include <R3E/objects/AbstractPhysicalObject.h>

//
// AbstractPhysicalObject
//

namespace r3e {
    AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f& position,
                                                   const sf::Vector2f& size,
                                                   const Collision::Area& c_area,
                                                   sf::Texture* texture,
                                                   short int frames_number,
                                                   float frame_duration) :
            AbstractDrawableObject(position, size, texture, frames_number, frame_duration),
            c_area_(c_area) {}

    const Collision::Area& AbstractPhysicalObject::getCollisionArea() const
    {
        return c_area_;
    }

    void AbstractPhysicalObject::changeCollisionArea(const Collision::Area& area)
    {
        c_area_ = area;
    }

//
// StaticObject
//

    StaticObject::StaticObject(const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               const Collision::Area& c_area,
                               sf::Texture* texture,
                               short int frames_number,
                               float frame_duration) :
            AbstractPhysicalObject(position, size, c_area, texture, frames_number, frame_duration) {}

    bool StaticObject::update(float time_elapsed) { return true; }

//
// DynamicObject
//

    DynamicObject::DynamicObject(const sf::Vector2f& position,
                                 const sf::Vector2f& velocity,
                                 const sf::Vector2f& size,
                                 const Collision::Area& c_area,
                                 sf::Texture* texture,
                                 short int frames_number,
                                 float frame_duration,
                                 const float acceleration) :
            StaticObject(position, size, c_area, texture, frames_number, frame_duration),
            curr_v_(velocity), set_v_(velocity),
            acceleration_(acceleration) {}

    const sf::Vector2f& DynamicObject::getVelocity() const
    {
        return curr_v_;
    }

    void DynamicObject::setVelocity(const sf::Vector2f& velocity)
    {
        set_v_ = velocity;
    }

    void DynamicObject::setVelocity(const float x, const float y)
    {
        set_v_.x = x;
        set_v_.y = y;
    }

    void DynamicObject::setForcedVelocity(const sf::Vector2f& velocity)
    {
        curr_v_ = velocity;
    }

    bool DynamicObject::update(float time_elapsed)
    {
        if (utils::num::isNearlyEqual(set_v_.x, 0.0f, 0.01f) &&
            utils::num::isNearlyEqual(curr_v_.x, 0.0f, 0.05f))
        {
            curr_v_.x = 0.0f;
        }
        else
        {
            curr_v_.x = curr_v_.x - std::copysign(acceleration_ * time_elapsed, curr_v_.x - set_v_.x);
        }
        if (utils::num::isNearlyEqual(set_v_.y, 0.0f, 0.01f) &&
            utils::num::isNearlyEqual(curr_v_.y, 0.0f, 0.05f))
        {
            curr_v_.y = 0.0f;
        }
        else
        {
            curr_v_.y = curr_v_.y - std::copysign(acceleration_ * time_elapsed, curr_v_.y - set_v_.y);
        }

        this->setPosition(this->getPosition() + curr_v_ * time_elapsed);

        return true;
    }

//
// HoveringObject
//

    HoveringObject::HoveringObject(const sf::Vector2f& position,
                                   const sf::Vector2f& velocity,
                                   const sf::Vector2f& size,
                                   const Collision::Area& c_area,
                                   sf::Texture* texture,
                                   short int frames_number,
                                   float frame_duration,
                                   float acceleration) :
            DynamicObject(position, velocity, size, c_area, texture, frames_number, frame_duration, acceleration) {}

} // namespace r3e