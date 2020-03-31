//
// Created by jul3x on 20.03.20.
//


#ifndef RAG3_COMMON_DECORATION_H
#define RAG3_COMMON_DECORATION_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

using namespace r3e;

class Decoration : public AbstractDrawableObject, public Identifiable, public Unique {
public:
    Decoration(const sf::Vector2f& position, const std::string& id, int u_id = -1);
};

#endif // RAG3_COMMON_DECORATION_H
