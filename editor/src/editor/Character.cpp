//
// Created by jul3x on 10.03.20.
//

#include <editor/ResourceManager.h>
#include <editor/Character.h>

using namespace editor;

Character::Character(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("characters", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("characters", id), "size_y")},
                               &RM.getTexture("characters/" + id))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("characters", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("characters", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("characters", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("characters", id), "map_offset_y")));
}
