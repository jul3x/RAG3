//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_EDITOR_CHARACTER_H
#define RAG3_EDITOR_CHARACTER_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class Character : public AbstractDrawableObject {
    public:
        Character(const sf::Vector2f& position, const std::string& id);
    };
} // namespace editor

#endif // RAG3_EDITOR_CHARACTER_H
