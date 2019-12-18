//
// Created by jul3x on 30.09.19.
//


#ifndef RAG3_GAME_ENVIRONMENT_MAP_H
#define RAG3_GAME_ENVIRONMENT_MAP_H

#include <list>

#include <game/characters/Enemy.h>
#include <game/environment/Obstacle.h>
#include <game/environment/Decoration.h>
#include <engine/objects/AbstractDrawableObject.h>


class Map : public AbstractDrawableObject {
public:
    Map() = default;

    void loadMap(const std::string& name);

    std::list<Obstacle>& getObstacles();

    std::list<Obstacle*>& getVisibleObstacles();

    std::list<Enemy>& getEnemies();

    std::list<Enemy*>& getVisibleEnemies();

    void setVisibility(const sf::View& view) override;

    void spawnDecoration(const sf::Vector2f& pos, Decoration::Type which);

    bool update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::list<Obstacle> obstacles_;
    std::list<Obstacle*> visible_obstacles_;

    std::list<Enemy> enemies_;
    std::list<Enemy*> visible_enemies_;

    std::list<Decoration> decorations_;
};

#endif // RAG3_GAME_ENVIRONMENT_MAP_H