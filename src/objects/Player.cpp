//
// Created by jprolejko on 30.09.19.
//

#include <objects/Player.h>

Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        AbstractPhysicalObject(position,
                               velocity,
                               {SIZE_X_, SIZE_Y_},
                               "player",
                               CFG.getFloat("player_max_acceleration"),
                               false) {}
