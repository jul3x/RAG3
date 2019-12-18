//
// Created by jprolejko on 21.11.19.
//


#ifndef RAG3_GAME_WEAPONS_NOWEAPON_H
#define RAG3_GAME_WEAPONS_NOWEAPON_H

#include <game/weapons/AbstractWeapon.h>


class NoWeapon : public AbstractWeapon  {
public:
    NoWeapon();

    virtual sf::Vector2f use();

};

#endif // RAG3_GAME_WEAPONS_NOWEAPON_H