//
// Created by jul3x on 21.11.19.
//


#ifndef RAG3_GAME_WEAPONS_NOWEAPON_H
#define RAG3_GAME_WEAPONS_NOWEAPON_H

#include <game/weapons/AbstractWeapon.h>


class NoWeapon : public AbstractWeapon {
public:
    NoWeapon();

    sf::Vector2f use() override;

};

#endif // RAG3_GAME_WEAPONS_NOWEAPON_H