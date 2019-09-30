//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_OBSTACLE_H
#define RAG3_OBJECTS_OBSTACLE_H

#include <objects/AbstractPhysicalObject.h>


class Obstacle : public AbstractPhysicalObject {
public:
    Obstacle(const sf::Vector2f &position,
             const int type_number);

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;
};

#endif // RAG3_OBJECTS_OBSTACLE_H
