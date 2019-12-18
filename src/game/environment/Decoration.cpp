//
// Created by jul3x on 01.10.19.
//

#include <game/environment/Decoration.h>
#include <engine/system/AbstractResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position,
                       const int type_number) :
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &AbstractResourceManager::getInstance().getTexture(
                                       "decoration" + std::to_string(type_number))) {}

Decoration::Decoration(const sf::Vector2f& position,
                       const Type type_number) : Decoration(position, static_cast<const int>(type_number)) {}