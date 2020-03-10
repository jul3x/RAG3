//
// Created by jul3x on 30.09.19.
//

#include <R3E/system/Config.h>

#include <editor/Obstacle.h>
#include <editor/ResourceManager.h>

using namespace editor;

Obstacle::Obstacle(const sf::Vector2f& position,
                   const int type_number) :
        AbstractDrawableObject(position,
                     {SIZE_X_, SIZE_Y_},
                     &RM.getTexture("obstacle" + std::to_string(type_number)))
{
    // type_number determines if wall is solid (for now)
    this->changeOrigin({SIZE_X_ / 2.0f, SIZE_Y_ / 2.0f + OFFSET_Y_});
}
