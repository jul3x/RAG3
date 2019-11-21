//
// Created by jprolejko on 31.10.19.
//


#ifndef RAG3_OBJECTS_ENEMY_H
#define RAG3_OBJECTS_ENEMY_H

#include <objects/Character.h>


class Enemy : public Character {
public:
    Enemy(const sf::Vector2f &position,
          const sf::Vector2f &velocity);

    virtual bool update(float time_elapsed);

};

#endif // RAG3_OBJECTS_ENEMY_H