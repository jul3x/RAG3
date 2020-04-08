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

    void DynamicObject::setAcceleration(float acc)
    {
        acceleration_ = acc;
    }

    float DynamicObject::getAcceleration() const
    {
        return acceleration_;
    }

    bool DynamicObject::update(float time_elapsed)
    {
        auto steering = utils::geo::vectorLengthLimit(set_v_ - curr_v_, acceleration_ * time_elapsed);
        curr_v_ = curr_v_ + steering;

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