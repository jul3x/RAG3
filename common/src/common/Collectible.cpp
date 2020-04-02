//
// Created by jul3x on 17.03.20.
//

#include <common/ResourceManager.h>
#include <common/Collectible.h>


Collectible::Collectible(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("collectibles", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("collectibles", id), "size_y")},
                               &RM.getTexture("collectibles/" + id),
                               utils::getInt(RM.getObjectParams("collectibles", id), "frames_number"),
                               utils::getFloat(RM.getObjectParams("collectibles", id), "frame_duration"))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("collectibles", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("collectibles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("collectibles", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("collectibles", id), "map_offset_y")));
}
