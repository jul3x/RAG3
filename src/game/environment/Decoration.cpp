//
// Created by jprolejko on 01.10.19.
//

#include <game/environment/Decoration.h>
#include <engine/system/ResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position,
                       const int type_number) :
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               &ResourceManager::getInstance().getTexture(
                                       "decoration" + std::to_string(type_number))) {}

Decoration::Decoration(const sf::Vector2f& position,
                       const Type type_number) : Decoration(position, static_cast<const int>(type_number)) {}