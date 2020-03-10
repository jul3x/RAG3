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

std::list<Decoration>& Map::getDecorations()
{
    return decorations_;
}

std::list<Obstacle>& Map::getObstacles()
{
    return obstacles_;
}

std::list<Character>& Map::getCharacters()
{
    return characters_;
}

void Map::spawnDecoration(const sf::Vector2f& pos, int which)
{
    decorations_.emplace_back(pos, which);
}
