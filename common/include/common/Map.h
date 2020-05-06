//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_MAP_H
#define RAG3_COMMON_INCLUDE_COMMON_MAP_H

#include <list>

#include <R3E/utils/Geometry.h>
#include <R3E/ai/DataTypes.h>

#include <common/DecorationTile.h>
#include <common/ObstacleTile.h>
#include <common/NPC.h>
#include <common/Special.h>
#include <common/Obstacle.h>
#include <common/Decoration.h>


using namespace r3e;

class Map {
public:
    using Data = std::tuple<std::list<std::shared_ptr<ObstacleTile>>, std::list<std::shared_ptr<DecorationTile>>,
            std::list<std::shared_ptr<NPC>>, std::list<std::shared_ptr<Special>>,
            std::list<std::shared_ptr<Obstacle>>, std::list<std::shared_ptr<Decoration>>>;
    using TileMap = std::tuple<sf::Vector2f, std::vector<std::vector<float>>>;

    Map();

    bool clearMap();
    bool loadMap(const std::string& name);

    [[nodiscard]] const sf::Vector2f& getSize() const;
    ai::MapBlockage& getMapBlockage();

    template<class T>
    std::list<std::shared_ptr<T>>& getList();

    template<class T>
    T* spawn(const sf::Vector2f& pos, const std::string& id, bool check = false,
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
                !this->checkCollisions(pos, obstacles_, erase, max_z_index));
    }


    std::list<std::shared_ptr<ObstacleTile>> obstacles_tiles_;
    std::list<std::shared_ptr<NPC>> characters_;
    std::list<std::shared_ptr<DecorationTile>> decorations_tiles_;
    std::list<std::shared_ptr<Special>> specials_;
    std::list<std::shared_ptr<Decoration>> decorations_;
    std::list<std::shared_ptr<Obstacle>> obstacles_;

    ai::MapBlockage blocked_;

    sf::Vector2f size_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_MAP_H