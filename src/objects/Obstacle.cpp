//
// Created by jprolejko on 30.09.19.
//

#include <system/Config.h>

#include <objects/Obstacle.h>


Obstacle::Obstacle(const sf::Vector2f &position,
                   const int type_number) :
        StaticObject(position,
                     {SIZE_X_, SIZE_Y_},
                     "obstacle" + std::to_string(type_number)) {
    shots_left_ = type_number * CFG.getInt("obstacles_endurance_factor");
    // type_number determines if wall is solid (for now)
}

void Obstacle::getShot(const Bullet &bullet) {
    shots_left_ -= bullet.getDeadlyFactor();
}

bool Obstacle::updateObstacle(float time_elapsed) {
    return shots_left_ > 0;
}
