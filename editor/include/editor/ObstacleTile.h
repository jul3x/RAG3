//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_OBSTACLETILE_H
#define RAG3_EDITOR_OBSTACLETILE_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class ObstacleTile : public AbstractDrawableObject {
    public:
        ObstacleTile(const sf::Vector2f& position, const std::string& id);

        static constexpr float SIZE_X_ = 100.0f;
        static constexpr float SIZE_Y_ = 150.0f;

        static constexpr float OFFSET_Y_ = 25.0f;

    };
} // namespace editor

#endif // RAG3_EDITOR_OBSTACLETILE_H
