//
// Created by jul3x on 30.09.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_MAP_H
#define RAG3_GAME_ENVIRONMENT_MAP_H

#include <list>

#include <game/characters/Enemy.h>
#include <game/environment/Obstacle.h>
#include <game/environment/Decoration.h>


class Map {
public:
    Map() = default;

    void loadMap(const std::string& name);

    const sf::Vector2f& getSize() const;

    std::list<Decoration>& getDecorations();

    std::list<Obstacle>& getObstacles();

    std::list<Enemy>& getEnemies();

    Decoration* spawnDecoration(const sf::Vector2f& pos, Decoration::Type which);

    bool update(float time_elapsed);

private:
    std::list<Obstacle> obstacles_;
    std::list<Enemy> enemies_;
    std::list<Decoration> decorations_;

    sf::Vector2f size_;

};

#endif // RAG3_GAME_ENVIRONMENT_MAP_H