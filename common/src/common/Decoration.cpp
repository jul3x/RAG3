//
// Created by jul3x on 20.03.20.
//

#include <common/Decoration.h>
#include <common/ResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("decorations", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("decorations", id), "size_y")},
                               &RM.getTexture("decorations/" + id),
                               utils::getInt(RM.getObjectParams("decorations", id), "frames_number"),
                               utils::getFloat(RM.getObjectParams("decorations", id), "frame_duration"))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("decorations", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("decorations", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("decorations", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("decorations", id), "map_offset_y")));
}
