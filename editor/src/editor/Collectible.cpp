//
// Created by jul3x on 17.03.20.
//

#include <editor/ResourceManager.h>
#include <editor/Collectible.h>

using namespace editor;


Collectible::Collectible(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("collectibles", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("collectibles", id), "size_y")},
                               &RM.getTexture("collectibles/" + id))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("collectibles", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("collectibles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("collectibles", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("collectibles", id), "map_offset_y")));
}
