//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_DECORATION_H
#define RAG3_EDITOR_DECORATION_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class Decoration : public AbstractDrawableObject {
    public:
        Decoration(const sf::Vector2f& position,
                   int type);

        static constexpr float SIZE_X_ = 100.0f;
        static constexpr float SIZE_Y_ = 100.0f;

    };
} // namespace editor

#endif // RAG3_EDITOR_DECORATION_H
