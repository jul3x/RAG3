//
// Created by jul3x on 16.06.19.
//


#ifndef RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H
#define RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H

#include <deque>
#include <iostream>
#include <cmath>

#include "AbstractDrawableObject.h"
#include "CollisionArea.h"


namespace r3e {

    class Collisions;


    class AbstractPhysicalObject : public AbstractDrawableObject {
    public:
        AbstractPhysicalObject(const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               const Collision::Area& c_area,
                               sf::Texture* texture,
                               short int frames_number = 1,
                               float frame_duration = 0.0f);

        const Collision::Area& getCollisionArea() const;

        virtual bool update(float time_elapsed) = 0;

    private:
        Collision::Area c_area_;

    };

    class StaticObject : public AbstractPhysicalObject {
    public:
        StaticObject(const sf::Vector2f& position,
                     const sf::Vector2f& size,
                     const Collision::Area& c_area,
                     sf::Texture* texture,
                     short int frames_number,
                     float frame_duration);

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
                      const Collision::Area& c_area,
                      sf::Texture* texture,
                      short int frames_number,
                      float frame_duration,
                      const sf::Color& trail_color,
                      float acceleration);

        const sf::Vector2f& getVelocity() const;

        virtual void setVelocity(const sf::Vector2f& velocity);

        virtual void setVelocity(float x, float y);

        virtual void setForcedVelocity(const sf::Vector2f& velocity);

        bool update(float time_elapsed) override;

    protected:
        std::deque<sf::Vector2f> trail_;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        static constexpr size_t TRAIL_COUNT_ = 10;

        float acceleration_;

        sf::Vector2f curr_v_, set_v_;
        sf::Color trail_color_;
    };

    class HoveringObject : public DynamicObject {
    public:
        HoveringObject(const sf::Vector2f& position,
                       const sf::Vector2f& velocity,
                       const sf::Vector2f& size,
                       const Collision::Area& c_area,
                       sf::Texture* texture,
                       short int frames_number,
                       float frame_duration,
                       const sf::Color& trail_color,
                       float acceleration);
    };

} // namespace r3e


#endif // RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H
