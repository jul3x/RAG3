//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("specials", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("specials", id), "size_y")},
                               &RM.getTexture("specials/" + id))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("specials", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("specials", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("specials", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("specials", id), "map_offset_y")));
}
