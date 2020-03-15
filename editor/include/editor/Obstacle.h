//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_OBSTACLE_H
#define RAG3_EDITOR_OBSTACLE_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class Obstacle : public AbstractDrawableObject {
    public:
        Obstacle(const sf::Vector2f& position, int type);

        static constexpr float SIZE_X_ = 100.0f;
        static constexpr float SIZE_Y_ = 150.0f;

        static constexpr float OFFSET_Y_ = 25.0f;

    };
} // namespace editor

#endif // RAG3_EDITOR_OBSTACLE_H
