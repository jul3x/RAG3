//
// Created by jul3x on 19.03.20.
//


#ifndef RAG3_COMMON_SPECIAL_H
#define RAG3_COMMON_SPECIAL_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>

using namespace r3e;

class Special : public AbstractDrawableObject, public Identifiable {
public:
    Special(const sf::Vector2f& position, const std::string& id);
};

#endif // RAG3_COMMON_SPECIAL_H
