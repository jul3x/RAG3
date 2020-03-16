//
// Created by jul3x on 10.03.20.
//

#include <editor/ResourceManager.h>

#include <editor/Map.h>

using namespace editor;

void Map::loadMap(const std::string& name)
{

}

const sf::Vector2f& Map::getSize() const
{
    return size_;
}

std::list<DecorationTile>& Map::getDecorationsTiles()
{
    return decorations_tiles_;
}

std::list<ObstacleTile>& Map::getObstaclesTiles()
{
    return obstacles_tiles_;
}

std::list<Character>& Map::getCharacters()
{
    return characters_;
}

void Map::spawnDecorationTile(const sf::Vector2f& pos, const std::string& id)
{
    if (!this->checkCollisions(pos, decorations_tiles_) && !this->checkCollisions(pos, obstacles_tiles_))
    {
        decorations_tiles_.emplace_back(pos, id);
    }
}

void Map::spawnObstacleTile(const sf::Vector2f& pos, const std::string& id)
{
    if (!this->checkCollisions(pos, decorations_tiles_) && !this->checkCollisions(pos, obstacles_tiles_))
    {
        obstacles_tiles_.emplace_back(pos, id);
    }
}

void Map::removeTile(const sf::Vector2f& pos)
{
    (!this->checkCollisions(pos, decorations_tiles_, true) && !this->checkCollisions(pos, obstacles_tiles_, true));
}

std::pair<sf::Vector2f, sf::Vector2f> Map::getTileConstraints() const
{
    sf::Vector2f min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    sf::Vector2f max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};

    for (const auto& obs : obstacles_tiles_)
    {
        if (obs.getPosition().x < min.x)
            min.x = obs.getPosition().x;
        if (obs.getPosition().y < min.y)
            min.y = obs.getPosition().y;

        if (obs.getPosition().x > max.x)
            max.x = obs.getPosition().x;
        if (obs.getPosition().y > max.y)
            max.y = obs.getPosition().y;
    }

    for (const auto& dec : decorations_tiles_)
    {
        if (dec.getPosition().x < min.x)
            min.x = dec.getPosition().x;
        if (dec.getPosition().y < min.y)
            min.y = dec.getPosition().y;

        if (dec.getPosition().x > max.x)
            max.x = dec.getPosition().x;
        if (dec.getPosition().y > max.y)
            max.y = dec.getPosition().y;
    }

    return {min, max};
}

