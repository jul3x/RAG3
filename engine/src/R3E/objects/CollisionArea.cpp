//
// Created by jul3x on 08.01.20.
//

#include <R3E/objects/CollisionArea.h>


namespace r3e::collision {

    // Area
    Area::Area(Type type, float a, float b, const sf::Vector2f& offset) :
            type_(type), a_(a), b_(b), offset_pos_(offset)
    {
    }

    const Area::Type& Area::getType() const
    {
        return type_;
    }

    const sf::Vector2f& Area::getOffset() const
    {
        return offset_pos_;
    }

    float Area::getA() const
    {
        return a_;
    }

    float Area::getB() const
    {
        return b_;
    }

    // None
    None::None() : Area(Area::Type::None, 0.0f, 0.0f, {})
    {
    }

    // Box
    Box::Box(float a, float b, const sf::Vector2f& offset) : Area(Area::Type::Box, a, b, offset)
    {
    }

    // Circle
    Circle::Circle(float r, const sf::Vector2f& offset) : Area(Area::Type::Circle, r, 0.0f, offset)
    {
    }

} // namespace r3e::collision