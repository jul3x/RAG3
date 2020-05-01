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
                         characters_, specials_,
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

std::list<std::shared_ptr<NPC>>& Map::getNPCs()
{
    return characters_;
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

DecorationTile* Map::spawnDecorationTile(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || (!this->checkCollisions(pos, decorations_tiles_, false, max_z_index) && !this->checkCollisions(pos, obstacles_tiles_, false, max_z_index)))
    {
        decorations_tiles_.emplace_back(std::make_shared<DecorationTile>(pos, id));
        return decorations_tiles_.back().get();
    }
}

ObstacleTile* Map::spawnObstacleTile(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || (!this->checkCollisions(pos, decorations_tiles_, false, max_z_index) && !this->checkCollisions(pos, obstacles_tiles_, false, max_z_index)))
    {
        auto grid_pos = std::make_pair(static_cast<size_t>(pos.x / DecorationTile::SIZE_X_),
                                       static_cast<size_t>(pos.y / DecorationTile::SIZE_Y_));

        if (blocked_.blockage_.size() > grid_pos.first && blocked_.blockage_.at(0).size() > grid_pos.second)
            blocked_.blockage_.at(grid_pos.first).at(grid_pos.second) =
                    utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "endurance");

        obstacles_tiles_.emplace_back(std::make_shared<ObstacleTile>(pos, id));
        return obstacles_tiles_.back().get();
    }
}

NPC* Map::spawnCharacter(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || this->checkCollisionsObjects(pos, false, max_z_index))
    {
        characters_.emplace_back(std::make_shared<NPC>(pos, id));
        return characters_.back().get();
    }
}

Special* Map::spawnSpecial(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || this->checkCollisionsObjects(pos, false, max_z_index))
    {
        specials_.emplace_back(std::make_shared<Special>(pos, id));
        return specials_.back().get();
    }
}

Decoration* Map::spawnDecoration(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || this->checkCollisionsObjects(pos, false, max_z_index))
    {
        decorations_.emplace_back(std::make_shared<Decoration>(pos, id));
        return decorations_.back().get();
    }
}

Obstacle* Map::spawnObstacle(const sf::Vector2f& pos, const std::string& id, bool check, int max_z_index)
{
    if (!check || this->checkCollisionsObjects(pos, false, max_z_index))
    {
        auto grid_pos = std::make_pair(static_cast<size_t>(pos.x / DecorationTile::SIZE_X_),
                                       static_cast<size_t>(pos.y / DecorationTile::SIZE_Y_));

        if (blocked_.blockage_.size() > grid_pos.first && blocked_.blockage_.at(0).size() > grid_pos.second)
            blocked_.blockage_.at(grid_pos.first).at(grid_pos.second) =
                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "endurance");

        obstacles_.emplace_back(std::make_shared<Obstacle>(pos, id));
        return obstacles_.back().get();
    }
}

void Map::removeTile(const sf::Vector2f& pos, int max_z_index)
{
    (!this->checkCollisions(pos, decorations_tiles_, true, max_z_index) && !this->checkCollisions(pos, obstacles_tiles_, true, max_z_index));
}

void Map::removeObject(const sf::Vector2f& pos, int max_z_index)
{
    checkCollisionsObjects(pos, true, max_z_index);
}

std::tuple<std::string, std::string, int> Map::getObjectInfo(const sf::Vector2f& pos, int max_z_index)
{
    auto ch = getItemInfo(pos, characters_, max_z_index);
    if (ch != nullptr)
        return std::make_tuple("characters", ch->getId(), ch->getUniqueId());

    auto dec = getItemInfo(pos, decorations_, max_z_index);
    if (dec != nullptr)
        return std::make_tuple("decorations", dec->getId(), dec->getUniqueId());

    auto obs = getItemInfo(pos, obstacles_, max_z_index);
    if (obs != nullptr)
        return std::make_tuple("obstacles", obs->getId(), obs->getUniqueId());

    return std::make_tuple("", "", -1);
}

Special* Map::getSpecialObject(const sf::Vector2f& pos, int max_z_index)
{
    return getItemInfo(pos, specials_, max_z_index);
}

NPC* Map::getNPCObject(const sf::Vector2f& pos, int max_z_index)
{
    return getItemInfo(pos, characters_, max_z_index);
}

Special* Map::getSpecialObject(int id)
{
    return getObject(id, specials_);
}

Obstacle* Map::getObstacleObject(int id)
{
    return getObject(id, obstacles_);
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

