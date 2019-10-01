//
// Created by jprolejko on 30.09.19.
//

#include <system/ResourceManager.h>
#include <system/Engine.h>

#include <objects/Map.h>


void Map::loadMap(const std::string &name) {
    std::tie(obstacles_, decorations_) = ResourceManager::getInstance().getMap(name);
    visible_obstacles_.clear();
}

std::list<Obstacle>& Map::getObstacles() {
    return obstacles_;
}

std::list<Obstacle*>& Map::getVisibleObstacles() {
    return visible_obstacles_;
}

void Map::setVisibility(const sf::View &view) {
    for (auto it = obstacles_.begin(); it != obstacles_.end(); ++it)
    {
        it->setVisibility(view);
    }
    for (auto it = decorations_.begin(); it != decorations_.end(); ++it)
    {
        it->setVisibility(view);
    }
}

void Map::update(float time_elapsed) {
    visible_obstacles_.clear();
    for (auto it = obstacles_.begin(); it != obstacles_.end(); ++it)
    {
        if (it->isVisible())
        {
            visible_obstacles_.push_back(&(*it));

            if (!it->updateObstacle(time_elapsed))
            {
                // draw on this place destruction
                decorations_.emplace_back(it->getPosition(), 2);
                Engine::getInstance().spawnExplosionAnimation(it->getPosition(), 25.0f);

                auto next_it = std::next(it);
                obstacles_.erase(it);
                it = next_it;
            }
        }
    }
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto &decoration : decorations_)
    {
        target.draw(decoration, states);
    }
    for (const auto &obstacle : obstacles_)
    {
        target.draw(obstacle, states);
    }
}
