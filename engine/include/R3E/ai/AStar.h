//
// Created by jul3x on 27.01.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_AI_ASTAR_H
#define RAG3_ENGINE_INCLUDE_R3E_AI_ASTAR_H

#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <utility>
#include <vector>
#include <list>
#include <functional>

#include <SFML/System/Vector2.hpp>

#include <R3E/ai/DataTypes.h>


namespace r3e::ai {
    class AStar {
    public:
        class Node {
        public:
            std::pair<int, int> cord_;
            float f_score_, g_score_, h_score_;

            Node(std::pair<int, int> c, float f, float g, float h) : cord_(std::move(c)), f_score_(f), g_score_(g),
                                                                     h_score_(h)
            {
            }

            Node() = default;

            explicit Node(const sf::Vector2i& c) : cord_(c.x, c.y), f_score_(), g_score_(), h_score_()
            {
            }
        };

        struct NodeHash {
            std::size_t operator()(AStar::Node const& a) const noexcept
            {
                return std::hash<int>()(a.cord_.first) ^ (std::hash<int>()(a.cord_.second) << 1);
            }
        };

        static Path
        getSmoothedPath(const MapBlockage& map_blockage, const sf::Vector2f& start, const sf::Vector2f& goal,
                        const NeighbourFunction& func, size_t limit = 1e12, bool can_avoid_specials = true);

        static NeighboursVec EightNeighbours(const Grid& grid, const sf::Vector2<size_t>& pos, bool can_avoid_specials);

        static NeighboursVec FourNeighbours(const Grid& grid, const sf::Vector2<size_t>& pos, bool can_avoid_specials);

    private:
        static bool isBlocked(float value, bool can_avoid_specials);

        static void getSmoothedPath_(Path& path);

        static std::vector<Node>
        getPath(const std::vector<std::vector<float>>& grid, const sf::Vector2<size_t>& start,
                const sf::Vector2<size_t>& goal, const NeighbourFunction& func, size_t limit,
                bool can_avoid_specials = true);

        static float heuristic(const Node& start, const Node& goal);

        static std::vector<Node>
        reconstructPath(const std::unordered_map<Node, Node, NodeHash>& came_from, const Node& current_node);

    };

    bool operator==(const AStar::Node& a, const AStar::Node& b);

} // namespace r3e::ai

#endif //RAG3_ENGINE_INCLUDE_R3E_AI_ASTAR_H
