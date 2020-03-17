//
// Created by jul3x on 17.03.20.
//


#ifndef RAG3_EDITOR_WEAPON_H
#define RAG3_EDITOR_WEAPON_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class Weapon : public AbstractDrawableObject {
    public:
        Weapon(const sf::Vector2f& position, const std::string& id);
    };
} // namespace editor

#endif // RAG3_EDITOR_WEAPON_H
