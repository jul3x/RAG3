//
// Created by jprolejko on 30.09.19.
//

#include <engine/system/ResourceManager.h>
#include <engine/system/Engine.h>

#include <game/environment/Map.h>


void Map::loadMap(const std::string &name) {
    std::tie(obstacles_, decorations_) = ResourceManager::getInstance().getMap(name);

    enemies_.emplace_back(sf::Vector2f{1000.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{500.0f, 1000.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1000.0f, 500.0f}, sf::Vector2f{0.0f, 0.0f});
    enemies_.emplace_back(sf::Vector2f{1500.0f, 300.0f}, sf::Vector2f{0.0f, 0.0f});

    visible_obstacles_.clear();
    visible_enemies_.clear();
}

std::list<Obstacle>& Map::getObstacles() {
    return obstacles_;
}

std::list<Obstacle*>& Map::getVisibleObstacles() {
    return visible_obstacles_;
}

std::list<Enemy>& Map::getEnemies() {
    return enemies_;
}

std::list<Enemy*>& Map::getVisibleEnemies() {
    return visible_enemies_;
}

void Map::setVisibility(const sf::View &view) {
    for (auto &obstacle : obstacles_) {
        obstacle.setVisibility(view);
    }
    for (auto &decoration : decorations_) {
        decoration.setVisibility(view);
    }
    for (auto &enemy : enemies_) {
        enemy.setVisibility(view);
    }
}

void Map::spawnDecoration(const sf::Vector2f &pos, Decoration::Type which) {
    decorations_.emplace_back(pos, which);
}

bool Map::update(float time_elapsed) {
    visible_obstacles_.clear();
    visible_enemies_.clear();
    // TODO Make private function that encapsulates this logic
    for (auto it = obstacles_.begin(); it != obstacles_.end();)
    {
        bool do_increment = true;
        if (it->isVisible())
        {
            if (!it->update(time_elapsed))
            {
                // draw on this place destruction
                spawnDecoration(it->getPosition(), Decoration::Type::DestroyedWall);
                Engine::getInstance().spawnExplosionAnimation(it->getPosition(), 25.0f);

                auto next_it = std::next(it);
                obstacles_.erase(it);
                it = next_it;
                do_increment = false;
            }
            else
            {
                visible_obstacles_.push_back(&(*it));
            }
        }

        if (do_increment) ++it;
    }

    for (auto it = enemies_.begin(); it != enemies_.end();)
    {
        bool do_increment = true;
        if (it->isVisible())
        {
            if (!it->update(time_elapsed))
            {
                // draw on this place destruction
                spawnDecoration(it->getPosition(), Decoration::Type::Blood);
                Engine::getInstance().spawnExplosionAnimation(it->getPosition(), 25.0f);

                auto next_it = std::next(it);
                enemies_.erase(it);
                it = next_it;
                do_increment = false;
            }
            else
            {
                visible_enemies_.push_back(&(*it));
            }
        }

        if (do_increment) ++it;
    }

    return true;
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto &decoration : decorations_)
    {
        target.draw(decoration, states);
    }
    for (const auto &obstacle : visible_obstacles_)
    {
        target.draw(*obstacle, states);
    }
    for (const auto &enemy : visible_enemies_)
    {
        target.draw(*enemy, states);
    }
}
