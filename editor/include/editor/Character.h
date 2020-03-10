//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_EDITOR_CHARACTER_H
#define RAG3_EDITOR_CHARACTER_H

#include <R3E/system/Config.h>
#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/ai/AStar.h>


using namespace r3e;

namespace editor {
    class Character : public AbstractDrawableObject {
    public:
        Character(const sf::Vector2f& position);

    private:
        static constexpr float OFFSET_Y_ = 10.0f;

        static constexpr float SIZE_X_ = 49.0f;
        static constexpr float SIZE_Y_ = 120.0f;

    };
} // namespace editor

#endif // RAG3_EDITOR_CHARACTER_H
