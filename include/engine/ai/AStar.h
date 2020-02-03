//
// Created by jul3x on 27.01.2020.
//

#ifndef RAG3_ENGINE_AI_ASTAR_H
#define RAG3_ENGINE_AI_ASTAR_H

#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <vector>

#include <SFML/System/Vector2.hpp>


class AStar {

public:
    class Node {
    public:
        std::pair<int, int> cord;
        float f_score, g_score, h_score;
        Node(const std::pair<int, int> &c, float f, float g, float h) : cord(c), f_score(f), g_score(g), h_score(h) {}
        Node() = default;
        Node(const sf::Vector2i &c) : cord(c.x, c.y), f_score(), g_score(), h_score() {}
    };

    struct NodeHash {
        std::size_t operator()(AStar::Node const &a) const noexcept {
            return std::hash<int>()(a.cord.first) ^ (std::hash<int>()(a.cord.second) << 1);
        }
    };

    static std::vector<sf::Vector2f> getSmoothedPath(const std::vector<Node> &path, float scale_x, float scale_y);

    static std::vector<Node> makePath(const std::vector<std::vector<bool>> &grid, const sf::Vector2<size_t> &start, const sf::Vector2<size_t> &goal);

private:
    static float heuristic(const Node &start, const Node &goal);

    static std::vector<Node> reconstructPath(const std::unordered_map<Node, Node, NodeHash> &came_from, const Node &current_node);

};

bool operator== (const AStar::Node &a, const AStar::Node &b);


#endif //RAG3_ENGINE_AI_ASTAR_H
