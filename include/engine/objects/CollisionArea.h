//
// Created by jul3x on 08.01.20.
//

#ifndef RAG3_ENGINE_OBJECTS_COLLISIONAREA_H
#define RAG3_ENGINE_OBJECTS_COLLISIONAREA_H

#include <SFML/System/Vector2.hpp>


namespace Collision {

class Area {

public:
    enum class Type {
        None,
        Box,
        Circle
    };

    Area(Type type, float a, float b, const sf::Vector2f &offset) :
            type_(type), a_(a), b_(b), offset_pos_(offset) {}

    const Type& getType() const
    {
        return type_;
    }

    const sf::Vector2f getOffset() const
    {
        return offset_pos_;
    }

    float getA() const
    {
        return a_;
    }

    float getB() const
    {
        return b_;
    }

private:
    Type type_;
    sf::Vector2f offset_pos_;

    float a_, b_;

};

class None : public Area {
public:
    None() : Area(Area::Type::None, 0.0f, 0.0f, {}) {}

};

class Box : public Area {
public:
    Box(float a, float b, const sf::Vector2f& offset = {}) : Area(Area::Type::Box, a, b, offset) {}

};

class Circle : public Area {
public:
    Circle(float r, const sf::Vector2f& offset = {}) : Area(Area::Type::Box, r, 0.0f, offset) {}

};

}; // namespace collision

#endif // RAG3_ENGINE_OBJECTS_COLLISIONAREA_H
