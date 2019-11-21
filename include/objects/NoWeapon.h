//
// Created by jprolejko on 21.11.19.
//


#ifndef RAG3_OBJECTS_NOWEAPON_H
#define RAG3_OBJECTS_NOWEAPON_H

#include <objects/AbstractWeapon.h>


class NoWeapon : public AbstractWeapon  {
public:
    NoWeapon();

    virtual sf::Vector2f use();

};

#endif // RAG3_OBJECTS_NOWEAPON_H