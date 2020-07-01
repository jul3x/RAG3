//
// Created by jul3x on 05.05.20.
//


#ifndef RAG3_GAME_INCLUDE_MISC_EXPLOSION_H
#define RAG3_GAME_INCLUDE_MISC_EXPLOSION_H

#include <R3E/objects/AbstractPhysicalObject.h>

#include <common/Character.h>
#include <common/Obstacle.h>
#include <common/ObstacleTile.h>

using namespace r3e;

class Explosion : public HoveringObject {
public:
    Explosion(const sf::Vector2f& position, float r);

    bool update(float time_elapsed) override;

    void applyForce(Character* obj) const;
    void applyForce(Obstacle* obj) const;

private:
    float r_;

};

#endif //RAG3_GAME_INCLUDE_MISC_EXPLOSION_H