//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_PLAYER_H
#define RAG3_OBJECTS_PLAYER_H

#include <chrono>
#include <deque>

#include <system/Config.h>
#include <objects/AbstractPhysicalObject.h>


class Player : public DynamicObject {
public:
    Player(const sf::Vector2f &position,
           const sf::Vector2f &velocity);

    void shot();

private:
    static constexpr float SIZE_X_ = 70.0f;
    static constexpr float SIZE_Y_ = 70.0f;
    static constexpr float GUN_OFFSET_X_ = 40.0f;

    // TODO - make weapon class
    std::chrono::system_clock::time_point last_bullet_time_;
};

#endif // RAG3_OBJECTS_PLAYER_H
