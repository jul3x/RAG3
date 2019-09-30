//
// Created by jprolejko on 30.09.19.
//

#include <system/Config.h>

#include <objects/Obstacle.h>


Obstacle::Obstacle(const sf::Vector2f &position,
                   const int type_number) :
        AbstractPhysicalObject(position,
                               {},
                               {SIZE_X_, SIZE_Y_},
                               "obstacle" + std::to_string(type_number),
                               0.0f) {
    shots_left_ = type_number * CFG.getInt("obstacles_endurance_factor");
    // type_number determines if wall is solid (for now)
}

bool Obstacle::getShot() {
    --shots_left_;

    return shots_left_ > 0;
}