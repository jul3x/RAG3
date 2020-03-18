//
// Created by jul3x on 10.03.20.
//

#include <editor/DecorationTile.h>
#include <editor/ResourceManager.h>

using namespace editor;

DecorationTile::DecorationTile(const sf::Vector2f& position,
                               const std::string& id) :
        Identifiable(id),
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &RM.getTexture(
                                       "decorations_tiles/" + id)) {}
