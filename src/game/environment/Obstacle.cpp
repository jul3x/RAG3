//
// Created by jul3x on 30.09.19.
//

#include <engine/system/Config.h>

#include <game/environment/Obstacle.h>
#include <game/misc/ResourceManager.h>


Obstacle::Obstacle(const sf::Vector2f& position,
                   const int type_number) :
        StaticObject(position,
                     {SIZE_X_, SIZE_Y_},
                     Collision::Box(COLLISION_SIZE_X_, COLLISION_SIZE_Y_),
                     &RM.getTexture("obstacle" + std::to_string(type_number))),
        Shootable(type_number * CFG.getInt("obstacles_endurance_factor"))
{
    // type_number determines if wall is solid (for now)
    this->changeOrigin({SIZE_X_ / 2.0f, SIZE_Y_ / 2.0f + COLLISION_OFFSET_Y_});
}

bool Obstacle::update(float time_elapsed)
{
    return life_ > 0;
}
