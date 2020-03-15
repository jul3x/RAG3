//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_MAP_H
#define RAG3_EDITOR_MAP_H

#include <list>

#include <R3E/utils/Geometry.h>

#include <editor/DecorationTile.h>
#include <editor/ObstacleTile.h>
#include <editor/Character.h>

using namespace r3e;

namespace editor {
    class Map {
    public:
        Map() = default;

        void loadMap(const std::string& name);

        const sf::Vector2f& getSize() const;

        std::list<DecorationTile>& getDecorationsTiles();

        std::list<ObstacleTile>& getObstaclesTiles();

        std::list<Character>& getCharacters();

        void spawnDecorationTile(const sf::Vector2f& pos, const std::string& id);

        void spawnObstacleTile(const sf::Vector2f& pos, const std::string& id);

    private:
        template<class T>
        bool checkCollisions(const sf::Vector2f& pos, const std::list<T>& objs)
        {
            for (const auto& obj : objs)
            {
                if (utils::geo::isPointInRectangle(
                        pos, obj.getPosition() - sf::Vector2f(DecorationTile::SIZE_X_ / 2.0f, DecorationTile::SIZE_Y_ / 2.0f),
                        {DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_}))
                {
                    return true;
                }
            }
            return false;
        }

        std::list<ObstacleTile> obstacles_tiles_;
        std::list<Character> characters_;
        std::list<DecorationTile> decorations_tiles_;

        sf::Vector2f size_;

    };
} // namespace editor

#endif // RAG3_EDITOR_MAP_H