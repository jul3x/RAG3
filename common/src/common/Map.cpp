//
// Created by jul3x on 10.03.20.
//

#include <common/ResourceManager.h>

#include <common/Map.h>


Map::Map()
{
}

bool Map::clearMap()
{
    obstacles_tiles_.clear();
    decorations_tiles_.clear();
    characters_.clear();
    collectibles_.clear();
    specials_.clear();
    obstacles_.clear();
    decorations_.clear();

    return true;
}

bool Map::loadMap(const std::string& name)
{
    try
    {
        std::forward_as_tuple(
                std::tie(obstacles_tiles_, decorations_tiles_,
                         characters_, collectibles_, specials_,
                         obstacles_, decorations_),
                std::tie(size_, blocked_.blockage_)) = ResourceManager::getMap(name);

        blocked_.scale_x_ = DecorationTile::SIZE_X_;
        blocked_.scale_y_ = DecorationTile::SIZE_X_;

        return true;
    }
    catch (const std::logic_error& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return false;
}

const sf::Vector2f& Map::getSize() const
{
    return size_;
}

ai::MapBlockage& Map::getMapBlockage()
{
    return blocked_;
}

std::list<std::shared_ptr<DecorationTile>>& Map::getDecorationsTiles()
{
    return decorations_tiles_;
}

std::list<std::shared_ptr<ObstacleTile>>& Map::getObstaclesTiles()
{
    return obstacles_tiles_;
}

std::list<std::shared_ptr<Enemy>>& Map::getCharacters()
{
    return characters_;
}

std::list<std::shared_ptr<Collectible>>& Map::getCollectibles()
{
    return collectibles_;
}

std::list<std::shared_ptr<Special>>& Map::getSpecials()
{
    return specials_;
}

std::list<std::shared_ptr<Decoration>>& Map::getDecorations()
{
    return decorations_;
}

std::list<std::shared_ptr<Obstacle>>& Map::getObstacles()
{
    return obstacles_;
}

void Map::spawnDecorationTile(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || (!this->checkCollisions(pos, decorations_tiles_) && !this->checkCollisions(pos, obstacles_tiles_)))
    {
        decorations_tiles_.emplace_back(std::make_shared<DecorationTile>(pos, id));
    }
}

void Map::spawnObstacleTile(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || (!this->checkCollisions(pos, decorations_tiles_) && !this->checkCollisions(pos, obstacles_tiles_)))
    {
        obstacles_tiles_.emplace_back(std::make_shared<ObstacleTile>(pos, id));
    }
}

void Map::spawnWeapon(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || this->checkCollisionsObjects(pos))
    {
        collectibles_.emplace_back(std::make_shared<Collectible>(pos, id));
    }
}

void Map::spawnCharacter(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || this->checkCollisionsObjects(pos))
    {
        characters_.emplace_back(std::make_shared<Enemy>(pos, id));
    }
}

void Map::spawnSpecial(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || this->checkCollisionsObjects(pos))
    {
        specials_.emplace_back(std::make_shared<Special>(pos, id));
    }
}

void Map::spawnDecoration(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || this->checkCollisionsObjects(pos))
    {
        decorations_.emplace_back(std::make_shared<Decoration>(pos, id));
    }
}

void Map::spawnObstacle(const sf::Vector2f& pos, const std::string& id, bool check)
{
    if (!check || this->checkCollisionsObjects(pos))
    {
        obstacles_.emplace_back(std::make_shared<Obstacle>(pos, id));
    }
}

void Map::removeTile(const sf::Vector2f& pos)
{
    (!this->checkCollisions(pos, decorations_tiles_, true) && !this->checkCollisions(pos, obstacles_tiles_, true));
}

void Map::removeObject(const sf::Vector2f& pos)
{
    checkCollisionsObjects(pos, true);
}

std::pair<sf::Vector2<size_t>, sf::Vector2f> Map::getTileConstraints() const
{
    sf::Vector2f min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    sf::Vector2f max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};

    for (const auto& obs : obstacles_tiles_)
    {
        if (obs->getPosition().x < min.x)
            min.x = obs->getPosition().x;
        if (obs->getPosition().y < min.y)
            min.y = obs->getPosition().y;

        if (obs->getPosition().x > max.x)
            max.x = obs->getPosition().x;
        if (obs->getPosition().y > max.y)
            max.y = obs->getPosition().y;
    }

    for (const auto& dec : decorations_tiles_)
    {
        if (dec->getPosition().x < min.x)
            min.x = dec->getPosition().x;
        if (dec->getPosition().y < min.y)
            min.y = dec->getPosition().y;

        if (dec->getPosition().x > max.x)
            max.x = dec->getPosition().x;
        if (dec->getPosition().y > max.y)
            max.y = dec->getPosition().y;
    }

    if (obstacles_tiles_.empty() && decorations_tiles_.empty()) return {{0, 0}, {0.0f, 0.0f}};

    return {sf::Vector2<size_t>(static_cast<size_t>((max.x - min.x) / DecorationTile::SIZE_X_) + 1,
                                static_cast<size_t>((max.y - min.y) / DecorationTile::SIZE_Y_) + 1),
            min};
}

