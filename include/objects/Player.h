//
// Created by jprolejko on 03.11.19.
//


#ifndef RAG3_OBJECTS_PLAYER_H
#define RAG3_OBJECTS_PLAYER_H

#include <objects/Character.h>


class Player : public Character {
public:
    Player(const sf::Vector2f &position,
           const sf::Vector2f &velocity);

};

#endif // RAG3_OBJECTS_PLAYER_H
