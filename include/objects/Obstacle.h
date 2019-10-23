//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_OBSTACLE_H
#define RAG3_OBJECTS_OBSTACLE_H

#include <objects/AbstractPhysicalObject.h>
#include <objects/Bullet.h>


class Obstacle : public StaticObject {
public:
    Obstacle(const sf::Vector2f &position,
             const int type_number);

    void getShot(const Bullet &bullet);
    bool updateObstacle(float time_elapsed);

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

private:
    int shots_left_;

};

#endif // RAG3_OBJECTS_OBSTACLE_H
