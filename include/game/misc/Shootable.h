//
// Created by jul3x on 19.12.2019.
//

#ifndef RAG3_SHOOTABLE_H
#define RAG3_SHOOTABLE_H

#include <game/weapons/Bullet.h>


class Shootable {
public:
    Shootable() = delete;

    Shootable(int life) : life_(life) {}

    virtual void getShot(const Bullet& bullet)
    {
        life_ -= bullet.getDeadlyFactor();
        life_ = life_ < 0 ? 0 : life_;
    }

protected:
    int life_;
};


#endif //RAG3_SHOOTABLE_H
