//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_MAP_H
#define RAG3_EDITOR_MAP_H

#include <list>

#include <editor/Decoration.h>
#include <editor/Obstacle.h>
#include <editor/Character.h>

using namespace r3e;

namespace editor {
    class Map {
    public:
        Map() = default;

        void loadMap(const std::string& name);

        const sf::Vector2f& getSize() const;

        std::list<Decoration>& getDecorations();

        std::list<Obstacle>& getObstacles();

        std::list<Character>& getCharacters();

        void spawnDecoration(const sf::Vector2f& pos, int type);

    private:
        std::list<Obstacle> obstacles_;
        std::list<Character> characters_;
        std::list<Decoration> decorations_;

        sf::Vector2f size_;

    };
} // namespace editor

#endif // RAG3_EDITOR_MAP_H