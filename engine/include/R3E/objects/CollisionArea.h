//
// Created by jul3x on 08.01.20.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_COLLISIONAREA_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_COLLISIONAREA_H

#include <SFML/System/Vector2.hpp>


namespace r3e::collision {
    class Area {

    public:
        enum class Type {
            None,
            Box,
            Circle
        };

        Area(Type type, float a, float b, const sf::Vector2f& offset);

        [[nodiscard]] const Type& getType() const;

        [[nodiscard]] const sf::Vector2f& getOffset() const;

        [[nodiscard]] float getA() const;

        [[nodiscard]] float getB() const;

    private:
        Type type_;
        sf::Vector2f offset_pos_;

        float a_, b_;

    };

    class None : public Area {
    public:
        None();

    };

    class Box : public Area {
    public:
        explicit Box(float a, float b, const sf::Vector2f& offset = {});

    };

    class Circle : public Area {
    public:
        explicit Circle(float r, const sf::Vector2f& offset = {});

    };

} // namespace r3e::collision

#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_COLLISIONAREA_H
