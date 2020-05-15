//
// Created by jul3x on 30.09.19.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/objects/AbstractPhysicalObject.h>

//
// AbstractPhysicalObject
//

namespace r3e {
    AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f& position,
                                                   const sf::Vector2f& size,
                                                   const collision::Area& c_area,
                                                   sf::Texture* texture,
                                                   int z_index,
                                                   short int frames_number,
                                                   float frame_duration) :
            AbstractDrawableObject(position, size, texture, z_index, frames_number, frame_duration),
            c_area_(c_area) {}

    const collision::Area& AbstractPhysicalObject::getCollisionArea() const
    {
        return c_area_;
    }

    void AbstractPhysicalObject::changeCollisionArea(const collision::Area& area)
    {
        c_area_ = area;
    }

//
// StaticObject
//

    StaticObject::StaticObject(const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               const collision::Area& c_area,
                               sf::Texture* texture,
                               int z_index,
                               short int frames_number,
                               float frame_duration) :
            AbstractPhysicalObject(position, size, c_area, texture, z_index, frames_number, frame_duration) {}

    bool StaticObject::update(float time_elapsed) { return true; }

//
// DynamicObject
//

    DynamicObject::DynamicObject(const sf::Vector2f& position,
                                 const sf::Vector2f& velocity,
                                 const sf::Vector2f& size,
                                 const collision::Area& c_area,
                                 sf::Texture* texture,
                                 int z_index,
                                 short int frames_number,
                                 float frame_duration,
                                 const float acceleration) :
            StaticObject(position, size, c_area, texture, z_index,  frames_number, frame_duration),
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
        auto steering = utils::geo::vectorLengthLimit(acceleration_ * (set_v_ - curr_v_) * time_elapsed, acceleration_);
        curr_v_ = curr_v_ + steering;

        for (auto it = steering_forces_.begin(); it != steering_forces_.end();)
        {
            bool do_increment = true;

            it->second -= time_elapsed;

            if (it->second < 0.0f)
            {
                auto next_it = std::next(it);

                steering_forces_.erase(it);
                it = next_it;
                do_increment = false;
            }


            curr_v_ = curr_v_ + it->first * time_elapsed;

            if (do_increment) ++it;
        }

        this->setPosition(this->getPosition() + curr_v_ * time_elapsed);

        return true;
    }

    void DynamicObject::addSteeringForce(const sf::Vector2f& force, float time)
    {
        steering_forces_.emplace_back(force, time);
    }

//
// HoveringObject
//

    HoveringObject::HoveringObject(const sf::Vector2f& position,
                                   const sf::Vector2f& velocity,
                                   const sf::Vector2f& size,
                                   const collision::Area& c_area,
                                   sf::Texture* texture,
                                   int z_index,
                                   short int frames_number,
                                   float frame_duration,
                                   float acceleration) :
            DynamicObject(position, velocity, size, c_area, texture, z_index, frames_number, frame_duration, acceleration) {}

} // namespace r3e