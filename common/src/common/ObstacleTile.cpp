//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>

#include <common/ObstacleTile.h>
#include <common/ResourceManager.h>


ObstacleTile::ObstacleTile(const sf::Vector2f& position, const std::string& id) :

        Identifiable(id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "size_y")},
                               &RM.getTexture("obstacles_tiles/" + id))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("obstacles_tiles", id), "map_offset_y")));
}