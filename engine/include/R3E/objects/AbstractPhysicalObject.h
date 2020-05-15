//
// Created by jul3x on 16.06.19.
//


#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_ABSTRACTPHYSICALOBJECT_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_ABSTRACTPHYSICALOBJECT_H

#include <iostream>
#include <cmath>
#include <list>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/CollisionArea.h>


namespace r3e {

    class Collisions;

    class AbstractPhysicalObject : public AbstractDrawableObject {
    public:
        AbstractPhysicalObject(const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               const collision::Area& c_area,
                               sf::Texture* texture,
                               int z_index = 0,
                               short int frames_number = 1,
                               float frame_duration = 0.0f);

        const collision::Area& getCollisionArea() const;

        void changeCollisionArea(const collision::Area& area);

        virtual bool update(float time_elapsed) = 0;

    private:
        collision::Area c_area_;

    };

    class StaticObject : public AbstractPhysicalObject {
    public:
        StaticObject(const sf::Vector2f& position,
                     const sf::Vector2f& size,
                     const collision::Area& c_area,
                     sf::Texture* texture,
                     int z_index = 0,
                     short int frames_number = 1,
                     float frame_duration = 0.0f);

        bool update(float time_elapsed) override;

    protected:
        sf::Vector2<size_t> grid_position_;

        friend class Collisions;
    };

    class DynamicObject : public StaticObject {
    public:
        DynamicObject(const sf::Vector2f& position,
                      const sf::Vector2f& velocity,
                      const sf::Vector2f& size,
                      const collision::Area& c_area,
                      sf::Texture* texture,
                      int z_index,
                      short int frames_number,
                      float frame_duration,
                      float acceleration);

        const sf::Vector2f& getVelocity() const;
        float getAcceleration() const;

        virtual void setVelocity(const sf::Vector2f& velocity);
        virtual void setVelocity(float x, float y);
        virtual void setForcedVelocity(const sf::Vector2f& velocity);
        virtual void setAcceleration(float acc);

        virtual void addSteeringForce(const sf::Vector2f& force, float time);

        bool update(float time_elapsed) override;

    private:
        float acceleration_;
        std::list<std::pair<sf::Vector2f, float>> steering_forces_;

        sf::Vector2f curr_v_, set_v_;
    };

    class HoveringObject : public DynamicObject {
    public:
        HoveringObject(const sf::Vector2f& position,
                       const sf::Vector2f& velocity,
                       const sf::Vector2f& size,
                       const collision::Area& c_area,
                       sf::Texture* texture,
                       int z_index,
                       short int frames_number,
                       float frame_duration,
                       float acceleration);
    };

} // namespace r3e


#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_ABSTRACTPHYSICALOBJECT_H
