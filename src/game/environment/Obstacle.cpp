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
                     Collision::Box(SIZE_X_, SIZE_Y_),
                     &ResourceManager::getInstance().getTexture("obstacle" + std::to_string(type_number))),
        Shootable(type_number * CFG.getInt("obstacles_endurance_factor"))
{
    // type_number determines if wall is solid (for now)
}

bool Obstacle::update(float time_elapsed)
{
    return life_ > 0;
}
