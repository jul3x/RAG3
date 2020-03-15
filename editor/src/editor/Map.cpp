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
        std::cout << "Decoration tile placed!" << std::endl;
    }
    else
    {
        std::cout << "Cannot place decoration tile!" << std::endl;
    }
}

void Map::spawnObstacleTile(const sf::Vector2f& pos, const std::string& id)
{
    if (!this->checkCollisions(pos, decorations_tiles_) && !this->checkCollisions(pos, obstacles_tiles_))
    {
        obstacles_tiles_.emplace_back(pos, id);
        std::cout << "Obstacle tile placed!" << std::endl;
    }
    else
    {
        std::cout << "Cannot place obstacle tile!" << std::endl;
    }
}
