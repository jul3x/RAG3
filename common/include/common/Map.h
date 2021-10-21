//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_MAP_H
#define RAG3_COMMON_INCLUDE_COMMON_MAP_H

#include <list>

#include <R3E/utils/Geometry.h>
#include <R3E/ai/DataTypes.h>

#include <common/objects/DecorationTile.h>
#include <common/objects/ObstacleTile.h>
#include <common/characters/NPC.h>
#include <common/objects/Special.h>
#include <common/objects/Obstacle.h>
#include <common/objects/Decoration.h>
#include <common/objects/PlacedWeapon.h>


using namespace r3e;

class Map {
public:
    using Data = std::tuple<std::list<std::shared_ptr<ObstacleTile>>, std::list<std::shared_ptr<DecorationTile>>,
            std::list<std::shared_ptr<NPC>>, std::list<std::shared_ptr<Special>>,
            std::list<std::shared_ptr<Obstacle>>, std::list<std::shared_ptr<Decoration>>,
            std::list<std::shared_ptr<PlacedWeapon>>>;
    using TileMap = std::tuple<sf::Vector2f, std::vector<std::vector<float>>, j3x::Parameters>;

    Map();

    bool clearMap();
    bool loadMap(const std::string& name);

    [[nodiscard]] const sf::Vector2f& getSize() const;
    ai::MapBlockage& getMapBlockage();

    template<class T>
    std::list<std::shared_ptr<T>>& getList();

    template<class T>
    T* spawn(const sf::Vector2f& pos, float direction, const std::string& id, bool check = false,
             int max_z_index = std::numeric_limits<int>::infinity());

    void removeTile(const sf::Vector2f& pos, int max_z_index = std::numeric_limits<int>::infinity());
    void removeObject(const sf::Vector2f& pos, int max_z_index = std::numeric_limits<int>::infinity());

    std::tuple<std::string, std::string, int>
    getObjectInfo(const sf::Vector2f& pos, int max_z_index = std::numeric_limits<int>::infinity());

    template<class T>
    T* getObjectByPos(const sf::Vector2f& pos, int max_z_index = std::numeric_limits<int>::infinity());

    template<class T>
    T* getObjectById(int id);

    [[nodiscard]] std::pair<sf::Vector2<size_t>, sf::Vector2f> getTileConstraints() const;
    [[nodiscard]] const j3x::Parameters& getParams() const;
    [[nodiscard]] size_t getDigest();
    [[nodiscard]] const std::string& getMapName() const;
    [[nodiscard]] size_t getMapIndex() const;
    [[nodiscard]] const std::string& getNextMapName() const;
    [[nodiscard]] const j3x::List& getPreviousMapsAndCurrent() const;

    void setParams(const j3x::Parameters& params);
    void setMapName(const std::string& map);

    static void markBlocked(ai::Grid& blocked, const sf::Vector2f& pos, const sf::Vector2f& size, float value);

private:
    template<class T>
    inline bool checkCollisions(const sf::Vector2f& pos, std::list<T>& objs, bool erase = false,
                                int max_z_index = std::numeric_limits<int>::infinity())
    {
        std::list<typename std::list<T>::iterator> found;
        for (auto it = objs.begin(); it != objs.end(); ++it)
        {
            if ((*it)->getZIndex() <= max_z_index &&
                utils::geo::isPointInRectangle(pos, (*it)->getPosition() - sf::Vector2f(DecorationTile::SIZE_X_ / 2.0f,
                                                                                        DecorationTile::SIZE_Y_ / 2.0f),
                                               {DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_}))
            {
                found.emplace_back(it);
            }
        }

        if (found.empty())
            return false;

        if (erase)
            objs.erase(found.back());

        return true;
    }

    template<class T>
    T* getItemInfo(const sf::Vector2f& pos, std::list<std::shared_ptr<T>>& objs,
                   int max_z_index = std::numeric_limits<int>::infinity())
    {
        std::list<typename std::list<std::shared_ptr<T>>::iterator> found;

        for (auto it = objs.begin(); it != objs.end(); ++it)
        {
            if ((*it)->getZIndex() <= max_z_index &&
                utils::geo::isPointInRectangle(pos, (*it)->getPosition() - sf::Vector2f(DecorationTile::SIZE_X_ / 2.0f,
                                                                                        DecorationTile::SIZE_Y_ / 2.0f),
                                               {DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_}))
            {
                found.emplace_back(it);
            }
        }

        if (found.empty())
            return nullptr;

        return found.back()->get();
    }

    template<class T>
    T* getObject(int id, std::list<std::shared_ptr<T>>& objs)
    {
        for (auto it = objs.begin(); it != objs.end(); ++it)
        {
            if ((*it)->getUniqueId() == id)
            {
                return it->get();
            }
        }
        return nullptr;
    }

    inline bool checkCollisionsObjects(const sf::Vector2f& pos, bool erase = false,
                                       int max_z_index = std::numeric_limits<int>::infinity())
    {
        return (!this->checkCollisions(pos, characters_, erase, max_z_index) &&
                !this->checkCollisions(pos, specials_, erase, max_z_index) &&
                !this->checkCollisions(pos, decorations_, erase, max_z_index) &&
                !this->checkCollisions(pos, obstacles_, erase, max_z_index) &&
                !this->checkCollisions(pos, weapons_, erase, max_z_index));
    }

    std::list<std::shared_ptr<ObstacleTile>> obstacles_tiles_;
    std::list<std::shared_ptr<NPC>> characters_;
    std::list<std::shared_ptr<DecorationTile>> decorations_tiles_;
    std::list<std::shared_ptr<Special>> specials_;
    std::list<std::shared_ptr<Decoration>> decorations_;
    std::list<std::shared_ptr<Obstacle>> obstacles_;
    std::list<std::shared_ptr<PlacedWeapon>> weapons_;

    ai::MapBlockage blocked_;

    j3x::Parameters params_;
    sf::Vector2f size_;

    size_t digest_;
    std::string map_name_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_MAP_H