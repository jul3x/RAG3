//
// Created by jul3x on 10.03.20.
//

#include <common/DecorationTile.h>
#include <common/ResourceManager.h>


DecorationTile::DecorationTile(const sf::Vector2f& position,
                               const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &RM.getTexture("decorations_tiles/" + id),
                               utils::j3x::get<int>(RM.getObjectParams("decorations_tiles", id), "z_index"))
{
}
