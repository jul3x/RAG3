//
// Created by jul3x on 30.09.19.
//

#include <game/misc/ResourceManager.h>

#include <game/environment/Map.h>
#include <game/Game.h>
#include <iostream>

void Map::loadMap(const std::string& name) {
    sf::Vector2i size_i;
    std::tie(size_i, blocked_.blockage_, obstacles_, decorations_) = ResourceManager::getMap(name);
    blocked_.scale_x_ = Obstacle::SIZE_X_;
    blocked_.scale_y_ = Obstacle::SIZE_Y_;
    size_ = {size_i.x * Obstacle::SIZE_X_, size_i.y * Obstacle::SIZE_Y_};

    enemies_.emplace_back(sf::Vector2f{1400.0f, 1200.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{70.0f, 70.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1000.0f, 100.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1900.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{2000.0f, 2000.0f}, sf::Vector2f{0.0f, 0.0f});

}

const sf::Vector2f& Map::getSize() const
{
    return size_;
}

const ai::MapBlockage& Map::getMapBlockage() const
{
    return blocked_;
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

void Map::spawnDecoration(const sf::Vector2f& pos, Decoration::Type which)
{
    decorations_.emplace_back(pos, which);
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

            auto grid_pos  = std::make_pair(static_cast<size_t>(it->getPosition().x / Obstacle::SIZE_X_),
                                            static_cast<size_t>(it->getPosition().y / Obstacle::SIZE_Y_));
            blocked_.blockage_.at(grid_pos.first).at(grid_pos.second) = false;
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
