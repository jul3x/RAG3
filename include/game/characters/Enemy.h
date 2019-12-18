//
// Created by jul3x on 31.10.19.
//


#ifndef RAG3_GAME_CHARACTERS_ENEMY_H
#define RAG3_GAME_CHARACTERS_ENEMY_H

#include <game/characters/Character.h>


class Enemy : public Character {
public:
    Enemy(const sf::Vector2f& position,
          const sf::Vector2f& velocity);

    bool update(float time_elapsed) override;

};

#endif // RAG3_GAME_CHARACTERS_ENEMY_H
