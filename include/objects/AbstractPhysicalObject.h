//
// Created by jprolejko on 16.06.19.
//


#ifndef RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H
#define RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H

#include <iostream>
#include <cmath>

#include <objects/AbstractDrawableObject.h>


class AbstractPhysicalObject : public AbstractDrawableObject {
public:
    AbstractPhysicalObject() = default;

    AbstractPhysicalObject(const sf::Vector2f &position,
                           const sf::Vector2f &velocity,
                           const sf::Vector2f &size,
                           const std::string &texture_name,
                           const double acceleration,
                           const bool is_static = true);

    const bool isStatic() const;
    const sf::Vector2f& getVelocity() const;

    void setVelocity(const sf::Vector2f &velocity);
    void setVelocity(const float x, const float y);
    void setForcedVelocity(const sf::Vector2f &velocity);

    void update(float time_elapsed);

private:
    float acceleration_;

    sf::Vector2f last_position_;
    sf::Vector2f curr_v_, set_v_;
    bool is_static_;

};

#endif // RAG3_OBJECTS_ABSTRACTPHYSICALOBJECT_H
