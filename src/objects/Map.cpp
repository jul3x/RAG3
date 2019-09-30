//
// Created by jprolejko on 30.09.19.
//

#include <system/ResourceManager.h>

#include <objects/Map.h>


void Map::loadMap(const std::string &name) {
    obstacles_ = ResourceManager::getInstance().getMap(name);
}

std::list<Obstacle>& Map::getObstacles() {
    return obstacles_;
}

void Map::update(float time_elapsed) {
    // TODO
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto &obstacle : obstacles_)
    {
        target.draw(obstacle, states);
    }
}
