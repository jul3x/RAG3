//
// Created by jprolejko on 21.11.19.
//


#ifndef RAG3_OBJECTS_NOWEAPON_H
#define RAG3_OBJECTS_NOWEAPON_H

#include <objects/Weapon.h>


class NoWeapon : public Weapon  {
public:
    NoWeapon();

    virtual sf::Vector2f shot();

};

#endif // RAG3_OBJECTS_NOWEAPON_H