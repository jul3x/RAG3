//
// Created by jprolejko on 30.09.19.
//

#include <objects/Obstacle.h>


Obstacle::Obstacle(const sf::Vector2f &position,
                   const int type_number) :
        AbstractPhysicalObject(position,
                               {},
                               {SIZE_X_, SIZE_Y_},
                               "obstacle" + std::to_string(type_number),
                               0.0f) {}
