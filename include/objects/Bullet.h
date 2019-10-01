//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_OBJECTS_BULLET_H
#define RAG3_OBJECTS_BULLET_H

#include <chrono>

#include <objects/AbstractPhysicalObject.h>


class Bullet : public HoveringObject {
public:
    Bullet(const sf::Vector2f &position,
           const float direction,
           const float bullet_life);

    bool updateBullet(float time_elapsed);

private:
    static constexpr float SIZE_X_ = 10.0f;
    static constexpr float SIZE_Y_ = 4.0f;

    float life_;
    std::chrono::system_clock::time_point spawn_time_;
};

#endif // RAG3_OBJECTS_PLAYER_H
