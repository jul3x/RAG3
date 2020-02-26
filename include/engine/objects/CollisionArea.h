//
// Created by jul3x on 08.01.20.
//

#ifndef RAG3_ENGINE_OBJECTS_COLLISIONAREA_H
#define RAG3_ENGINE_OBJECTS_COLLISIONAREA_H

#include <SFML/System/Vector2.hpp>


namespace r3e {

    namespace Collision {

        class Area {

        public:
            enum class Type {
                None,
                Box,
                Circle
            };

            Area(Type type, float a, float b, const sf::Vector2f& offset);

            const Type& getType() const;

            const sf::Vector2f& getOffset() const;

            float getA() const;

            float getB() const;

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
            Box(float a, float b, const sf::Vector2f& offset = {});

        };

        class Circle : public Area {
        public:
            explicit Circle(float r, const sf::Vector2f& offset = {});

        };

    }; // namespace collision

} // namespace r3e

#endif // RAG3_ENGINE_OBJECTS_COLLISIONAREA_H
