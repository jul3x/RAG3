//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_MAP_H
#define RAG3_COMMON_MAP_H

#include <list>

#include <R3E/utils/Geometry.h>
#include <R3E/ai/DataTypes.h>

#include <common/DecorationTile.h>
#include <common/ObstacleTile.h>
#include <common/Enemy.h>
#include <common/Collectible.h>
#include <common/Special.h>


using namespace r3e;

class Map {
public:
    using Data = std::tuple<std::list<std::shared_ptr<ObstacleTile>>, std::list<std::shared_ptr<DecorationTile>>,
                            std::list<std::shared_ptr<Enemy>>, std::list<std::shared_ptr<Collectible>>,
                            std::list<std::shared_ptr<Special>>>;
    using TileMap = std::tuple<sf::Vector2f, std::vector<std::vector<float>>>;

    Map();

    bool clearMap();

    bool loadMap(const std::string& name);

    const sf::Vector2f& getSize() const;

    ai::MapBlockage& getMapBlockage();

    std::list<std::shared_ptr<DecorationTile>>& getDecorationsTiles();

    std::list<std::shared_ptr<ObstacleTile>>& getObstaclesTiles();

    std::list<std::shared_ptr<Enemy>>& getCharacters();

    std::list<std::shared_ptr<Collectible>>& getCollectibles();

    std::list<std::shared_ptr<Special>>& getSpecials();

    void spawnDecorationTile(const sf::Vector2f& pos, const std::string& id);

    void spawnObstacleTile(const sf::Vector2f& pos, const std::string& id);

    void spawnWeapon(const sf::Vector2f& pos, const std::string& id);

    void spawnCharacter(const sf::Vector2f& pos, const std::string& id);

    void spawnSpecial(const sf::Vector2f& pos, const std::string& id);

    void removeTile(const sf::Vector2f& pos);

    void removeObject(const sf::Vector2f& pos);

    std::pair<sf::Vector2<size_t>, sf::Vector2f> getTileConstraints() const;

private:
    template<class T>
    bool checkCollisions(const sf::Vector2f& pos, std::list<T>& objs, bool erase = false)
    {
        for (auto it = objs.begin(); it != objs.end(); ++it)
        {
            if (utils::geo::isPointInRectangle(
                    pos, (*it)->getPosition() - sf::Vector2f(DecorationTile::SIZE_X_ / 2.0f, DecorationTile::SIZE_Y_ / 2.0f),
                    {DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_}))
            {
                if (erase)
                    objs.erase(it);

                return true;
            }
        }
        return false;
    }


    std::list<std::shared_ptr<ObstacleTile>> obstacles_tiles_;
    std::list<std::shared_ptr<Enemy>> characters_;
    std::list<std::shared_ptr<Collectible>> collectibles_;
    std::list<std::shared_ptr<DecorationTile>> decorations_tiles_;
    std::list<std::shared_ptr<Special>> specials_;

    ai::MapBlockage blocked_;

    sf::Vector2f size_;

};

#endif // RAG3_COMMON_MAP_H