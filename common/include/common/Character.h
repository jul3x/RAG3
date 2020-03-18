//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_COMMON_CHARACTER_H
#define RAG3_COMMON_CHARACTER_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>


using namespace r3e;

class Character : public AbstractDrawableObject, public Identifiable {
public:
    Character(const sf::Vector2f& position, const std::string& id);
};

#endif // RAG3_COMMON_CHARACTER_H
