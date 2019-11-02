//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_OBJECTS_BULLET_H
#define RAG3_OBJECTS_BULLET_H

#include <chrono>

#include <objects/AbstractPhysicalObject.h>


struct BulletDescription {
    float speed_;
    float life_;
    int deadly_factor_;

    std::string name_;

    float size_x_;
    float size_y_;
};


class Bullet : public HoveringObject {
public:
    Bullet(const BulletDescription &description,
           const sf::Vector2f &position,
           const float direction);

    bool update(float time_elapsed);
    const int getDeadlyFactor() const;

private:
    static constexpr size_t TRAIL_COUNT_ = 4;

    float life_;
    int deadly_factor_;

    std::chrono::system_clock::time_point spawn_time_;
};

#endif // RAG3_OBJECTS_PLAYER_H
