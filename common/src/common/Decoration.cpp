//
// Created by jul3x on 20.03.20.
//

#include <common/Decoration.h>
#include <common/ResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        AbstractDrawableObject(position,
                               {utils::j3x::get<float>(RM.getObjectParams("decorations", id), "size_x"),
                                utils::j3x::get<float>(RM.getObjectParams("decorations", id), "size_y")},
                               &RM.getTexture("decorations/" + id),
                               utils::j3x::get<int>(RM.getObjectParams("decorations", id), "frames_number"),
                               utils::j3x::get<float>(RM.getObjectParams("decorations", id), "frame_duration"))
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("decorations", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("decorations", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("decorations", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("decorations", id), "map_offset_y")));
}