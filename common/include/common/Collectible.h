//
// Created by jul3x on 17.03.20.
//


#ifndef RAG3_COMMON_COLLECTIBLE_H
#define RAG3_COMMON_COLLECTIBLE_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>

using namespace r3e;

class Collectible : public AbstractDrawableObject, public Identifiable {
public:
    Collectible(const sf::Vector2f& position, const std::string& id);
};

#endif // RAG3_COMMON_COLLECTIBLE_H
