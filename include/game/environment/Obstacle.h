//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_OBSTACLE_H
#define RAG3_GAME_ENVIRONMENT_OBSTACLE_H

#include <engine/objects/AbstractPhysicalObject.h>
#include <game/weapons/Bullet.h>


class Obstacle : public StaticObject {
public:
    Obstacle(const sf::Vector2f& position,
             int type_number);

    void getShot(const Bullet& bullet);

    bool update(float time_elapsed) override;

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

private:
    int shots_left_;

};

#endif // RAG3_GAME_ENVIRONMENT_OBSTACLE_H
