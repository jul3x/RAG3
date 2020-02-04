//
// Created by jul3x on 27.01.2020.
//

#ifndef RAG3_ENGINE_AI_ASTAR_H
#define RAG3_ENGINE_AI_ASTAR_H

#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <utility>
#include <vector>
#include <list>

#include <SFML/System/Vector2.hpp>


namespace ai {
    struct MapBlockage {
        std::vector<std::vector<bool>> blockage_;
        float scale_x_, scale_y_;
    };

    class AStar {

    public:
        class Node {
        public:
            std::pair<int, int> cord;
            float f_score, g_score, h_score;

            Node(std::pair<int, int> c, float f, float g, float h) : cord(std::move(c)), f_score(f), g_score(g),
                                                                     h_score(h) {}

            Node() = default;

            explicit Node(const sf::Vector2i& c) : cord(c.x, c.y), f_score(), g_score(), h_score() {}
        };

        struct NodeHash {
            std::size_t operator()(AStar::Node const& a) const noexcept
            {
                return std::hash<int>()(a.cord.first) ^ (std::hash<int>()(a.cord.second) << 1);
            }
        };

        static std::list<sf::Vector2f>
        getSmoothedPath(const MapBlockage& map_blockage_, const sf::Vector2f& start, const sf::Vector2f& goal);

    private:
        static void getSmoothedPath_(std::list<sf::Vector2f>& path);

        static std::vector<Node> getPath(const std::vector<std::vector<bool>>& grid, const sf::Vector2<size_t>& start,
                                         const sf::Vector2<size_t>& goal);

        static float heuristic(const Node& start, const Node& goal);

        static std::vector<Node>
        reconstructPath(const std::unordered_map<Node, Node, NodeHash>& came_from, const Node& current_node);

    };

    bool operator==(const AStar::Node& a, const AStar::Node& b);

} // namespace ai


#endif //RAG3_ENGINE_AI_ASTAR_H
