//
// Created by jprolejko on 16.06.19.
//


#ifndef RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H
#define RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H

#include <deque>
#include <iostream>
#include <cmath>

#include <engine/objects/AbstractDrawableObject.h>


class AbstractPhysicalObject : public AbstractDrawableObject {
public:
    AbstractPhysicalObject() = default;

    AbstractPhysicalObject(const sf::Vector2f& position,
                           const sf::Vector2f& size,
                           sf::Texture* texture);

    virtual bool update(float time_elapsed) = 0;

};

class StaticObject : public AbstractPhysicalObject {
public:
    StaticObject() = default;

    StaticObject(const sf::Vector2f& position,
                 const sf::Vector2f& size,
                 sf::Texture* texture);

    bool update(float time_elapsed) override;

};

class DynamicObject : public StaticObject {
public:
    DynamicObject() = default;

    DynamicObject(const sf::Vector2f& position,
                  const sf::Vector2f& velocity,
                  const sf::Vector2f& size,
                  sf::Texture* texture,
                  float acceleration);

    const sf::Vector2f& getVelocity() const;

    void setVelocity(const sf::Vector2f& velocity);

    void setVelocity(float x, float y);

    void setForcedVelocity(const sf::Vector2f& velocity);

    void setVisibility(const sf::View& view) override;

    bool update(float time_elapsed) override;

protected:
    std::deque<sf::Vector2f> trail_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    float acceleration_;

    static constexpr size_t TRAIL_COUNT_ = 10;

    sf::Vector2f curr_v_, set_v_;

};

class HoveringObject : public DynamicObject {
public:
    HoveringObject() = default;

    HoveringObject(const sf::Vector2f& position,
                   const sf::Vector2f& velocity,
                   const sf::Vector2f& size,
                   sf::Texture* texture,
                   float acceleration);
};

#endif // RAG3_ENGINE_OBJECTS_ABSTRACTPHYSICALOBJECT_H
