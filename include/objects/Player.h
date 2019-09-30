//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_PLAYER_H
#define RAG3_OBJECTS_PLAYER_H

#include <system/Config.h>
#include <objects/AbstractPhysicalObject.h>


class Player : public AbstractPhysicalObject {
public:
    Player(const sf::Vector2f &position,
           const sf::Vector2f &velocity);

private:
    static constexpr float SIZE_X_ = 70.0f;
    static constexpr float SIZE_Y_ = 70.0f;
};

#endif // RAG3_OBJECTS_PLAYER_H
