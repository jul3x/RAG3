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
           const float bullet_life,
           const int deadly_factor);

    bool updateBullet(float time_elapsed);
    const int getDeadlyFactor() const;

private:
    static constexpr float SIZE_X_ = 10.0f;
    static constexpr float SIZE_Y_ = 4.0f;

    static constexpr size_t TRAIL_COUNT_ = 4;

    float life_;
    int deadly_factor_;
    std::chrono::system_clock::time_point spawn_time_;
};

#endif // RAG3_OBJECTS_PLAYER_H
