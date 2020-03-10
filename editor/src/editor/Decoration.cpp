//
// Created by jul3x on 10.03.20.
//

#include <editor/Decoration.h>
#include <editor/ResourceManager.h>

using namespace editor;

Decoration::Decoration(const sf::Vector2f& position,
                       const int type_number) :
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &RM.getTexture(
                                       "decoration" + std::to_string(type_number))) {}
