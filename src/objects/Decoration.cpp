//
// Created by jprolejko on 01.10.19.
//

#include <objects/Decoration.h>


Decoration::Decoration(const sf::Vector2f &position,
                       const int type_number) :
        AbstractDrawableObject(position,
                               {SIZE_X_, SIZE_Y_},
                               "decoration" + std::to_string(type_number)) {}
