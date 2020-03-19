//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_DECORATIONTILE_H
#define RAG3_COMMON_DECORATIONTILE_H

#include <string>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>


using namespace r3e;

class DecorationTile : public AbstractDrawableObject, public Identifiable {
public:
    enum class Type {
        Blood = 3,
        DestroyedWall = 2
    };

    DecorationTile(const sf::Vector2f& position,
                   const std::string& id);

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

};

#endif // RAG3_COMMON_DECORATIONTILE_H
