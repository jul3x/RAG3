//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_DECORATION_H
#define RAG3_GAME_ENVIRONMENT_DECORATION_H

#include <engine/objects/AbstractDrawableObject.h>


class Decoration : public AbstractDrawableObject {
public:
    enum class Type {
        Blood = 3,
        DestroyedWall = 2
    };


    Decoration(const sf::Vector2f& position,
               int type);

    Decoration(const sf::Vector2f& position,
               Type type);

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

};

#endif // RAG3_GAME_ENVIRONMENT_DECORATION_H
