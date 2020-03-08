//
// Created by jul3x on 30.09.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_OBSTACLE_H
#define RAG3_GAME_ENVIRONMENT_OBSTACLE_H

#include <engine/objects/AbstractPhysicalObject.h>
#include <game/weapons/Bullet.h>
#include <game/misc/Shootable.h>


using namespace r3e;

class Obstacle : public StaticObject, public Shootable {
public:
    Obstacle(const sf::Vector2f& position,
             int type_number);

    bool update(float time_elapsed) override;

    static constexpr float COLLISION_SIZE_X_ = 100.0f;
    static constexpr float COLLISION_SIZE_Y_ = 100.0f;
    static constexpr float COLLISION_OFFSET_Y_ = 25.0f;


    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 150.0f;

};

#endif // RAG3_GAME_ENVIRONMENT_OBSTACLE_H
