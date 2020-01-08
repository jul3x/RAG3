//
// Created by jul3x on 30.09.19.
//

#include <game/misc/ResourceManager.h>

#include <game/environment/Map.h>
#include <game/Game.h>


void Map::loadMap(const std::string& name)
{
    std::tie(obstacles_, decorations_) = ResourceManager::getMap(name);

    enemies_.emplace_back(sf::Vector2f{1000.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{500.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1000.0f, 500.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1500.0f, 300.0f}, sf::Vector2f{0.0f, 0.0f});
//    enemies_.emplace_back(sf::Vector2f{-1000.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
//    enemies_.emplace_back(sf::Vector2f{-500.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
//    enemies_.emplace_back(sf::Vector2f{-1000.0f, 500.0f}, sf::Vector2f{0.0f, 0.0f});
//    enemies_.emplace_back(sf::Vector2f{-1500.0f, 300.0f}, sf::Vector2f{0.0f, 0.0f});
}

std::list<Decoration>& Map::getDecorations()
{
    return decorations_;
}

std::list<Obstacle>& Map::getObstacles()
{
    return obstacles_;
}

std::list<Enemy>& Map::getEnemies()
{
    return enemies_;
}

Decoration* Map::spawnDecoration(const sf::Vector2f& pos, Decoration::Type which)
{
    decorations_.emplace_back(pos, which);
    return &decorations_.back();
}

bool Map::update(float time_elapsed)
{
    // TODO Make private function that encapsulates this logic
    for (auto it = obstacles_.begin(); it != obstacles_.end();)
    {
        bool do_increment = true;
        if (!it->update(time_elapsed))
        {
            // draw on this place destruction
            spawnDecoration(it->getPosition(), Decoration::Type::DestroyedWall);
            Game::get().spawnExplosionAnimation(it->getPosition(), 25.0f);

            auto next_it = std::next(it);
            Game::get().deleteStaticObject(&*it);
            obstacles_.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto it = enemies_.begin(); it != enemies_.end();)
    {
        bool do_increment = true;
        if (!it->update(time_elapsed))
        {
            // draw on this place destruction
            spawnDecoration(it->getPosition(), Decoration::Type::Blood);
            Game::get().spawnExplosionAnimation(it->getPosition(), 25.0f);

            auto next_it = std::next(it);
            Game::get().deleteDynamicObject(&*it);
            enemies_.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    return true;
}
