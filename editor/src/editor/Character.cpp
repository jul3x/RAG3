//
// Created by jul3x on 10.03.20.
//

#include <editor/ResourceManager.h>
#include <editor/Character.h>

using namespace editor;

Character::Character(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                      {SIZE_X_, SIZE_Y_},
                      &RM.getTexture("player_1"))
{
    this->changeOrigin({SIZE_X_ / 2.0f, SIZE_Y_ / 2.0f + OFFSET_Y_});
}
