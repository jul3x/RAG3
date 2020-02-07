//
// Created by jul3x on 30.09.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_OBSTACLE_H
#define RAG3_GAME_ENVIRONMENT_OBSTACLE_H

#include <engine/objects/AbstractPhysicalObject.h>
#include <game/weapons/Bullet.h>
#include <game/misc/Shootable.h>


class Obstacle : public StaticObject, public Shootable {
public:
    Obstacle(const sf::Vector2f& position,
             int type_number);

    bool update(float time_elapsed) override;

    static constexpr float SIZE_X_ = 30.0f;
    static constexpr float SIZE_Y_ = 30.0f;

};

#endif // RAG3_GAME_ENVIRONMENT_OBSTACLE_H
