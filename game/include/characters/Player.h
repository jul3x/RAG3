//
// Created by jul3x on 03.11.19.
//


#ifndef RAG3_GAME_CHARACTERS_PLAYER_H
#define RAG3_GAME_CHARACTERS_PLAYER_H

#include <characters/Character.h>


class Player : public Character {
public:
    Player(const sf::Vector2f& position,
           const sf::Vector2f& velocity);

    void setDead();

    bool isAlive() const;


private:
    bool is_alive_;
};

#endif // RAG3_GAME_CHARACTERS_PLAYER_H
