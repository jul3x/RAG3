//
// Created by jul3x on 20.03.20.
//


#ifndef RAG3_COMMON_DECORATION_H
#define RAG3_COMMON_DECORATION_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>

using namespace r3e;

class Decoration : public AbstractDrawableObject, public Identifiable {
public:
    Decoration(const sf::Vector2f& position, const std::string& id);
};

#endif // RAG3_COMMON_DECORATION_H
