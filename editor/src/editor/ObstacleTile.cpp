//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>

#include <editor/ObstacleTile.h>
#include <editor/ResourceManager.h>

using namespace editor;

ObstacleTile::ObstacleTile(const sf::Vector2f& position, const std::string& id) :
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &RM.getTexture("obstacles_tiles/" + id))
{
    // type_number determines if wall is solid (for now)
    this->changeOrigin({SIZE_X_ / 2.0f, SIZE_Y_ / 2.0f + OFFSET_Y_});
}
