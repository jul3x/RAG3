//
// Created by jul3x on 27.01.2020.
//

#ifndef RAG3_ENGINE_UTILS_ASTAR_H
#define RAG3_ENGINE_UTILS_ASTAR_H

#include <queue>
#include <map>
#include <unordered_set>
#include <set>
#include <limits>
#include <cmath>


namespace utils {
namespace astar {

    class Node {
    public:
        std::pair<int, int> cord;
        float f_score, g_score, h_score;
        Node(const std::pair<int, int> &c, float f, float g, float h) : cord(c), f_score(f), g_score(g), h_score(h) {}
    };

    bool operator< (const Node &a, const Node &b) {
        if (a.f_score < b.f_score) return true;
        if (a.f_score == b.f_score)
        {
            return a.cord < b.cord;
        }

        return false;
    }

    bool operator== (const Node &a, const Node &b) {
        return a.cord.first == b.cord.first && a.cord.second == b.cord.second;
    }

    float heuristic(const Node &start, const Node &goal)
    {
        return std::hypot(start.cord.first - goal.cord.first, start.cord.second - goal.cord.second);
    }

    std::vector<std::pair<int, int>> reconstruct_path(const std::map<std::pair<int, int>, std::pair<int, int>> &came_from, const std::pair<int, int> &current_node) {
        auto came_from_it = came_from.find(current_node);
        if (came_from_it != came_from.end())
        {
            std::vector<std::pair<int, int>> p = reconstruct_path(came_from, came_from_it->second);
            p.push_back(current_node);
            return p;
        }
        else
        {
            return {};
        }
    }

    std::vector<std::pair<int, int>> astar(const std::vector<std::vector<bool>> &grid, const sf::Vector2i &start, const sf::Vector2i &goal)
    {
        // TODO REIMPLEMENT EVERYTHING!

        std::set<std::pair<int, int>> closedset;
        std::set<Node> openset;

        static constexpr float INF = std::numeric_limits<float>::infinity();

        openset.insert(Node({start.x, start.y}, 0.0f, 0.0f, 0.0f));
        Node goal_node = Node({goal.x, goal.y}, INF, INF, INF);

        static std::vector<std::pair<int, int>> neighbours = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        int i = 0;
        std::map<std::pair<int, int>, std::pair<int, int>> came_from;
        while (!openset.empty())
        {
            auto x_it = openset.begin();
            Node x = *x_it;
            openset.erase(x_it);

            if (x == goal_node) {
                return reconstruct_path(came_from, goal_node.cord);
            }

            closedset.insert(x.cord);

            for (const auto &neigh : neighbours)
            {
                int cord_x = x.cord.first + neigh.first;
                int cord_y = x.cord.second + neigh.second;

                if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

                if (grid[cord_x][cord_y]) continue;

                Node y = Node({cord_x, cord_y}, INF, INF, INF);
                if (closedset.find(y.cord) != closedset.end()) {
                    continue;
                }

                float tentative_g_score = x.g_score + std::hypot(x.cord.first - y.cord.first, x.cord.second - y.cord.second);
                bool tentative_is_better = false;
                bool should_insert = false;

                auto y_it = openset.find(y);

                if (y_it == openset.end())
                {
                    should_insert = true;
                    y.h_score = heuristic(y, goal_node);
                    y.g_score = tentative_g_score;
                    y.f_score = y.g_score + y.h_score;
                    openset.insert(y);
                    tentative_is_better = true;
                }
                else if (tentative_g_score < y_it->g_score)
                {
                    Node new_y = Node({y_it->cord.first, y_it->cord.second}, y_it->f_score, y_it->g_score, y_it->h_score);
                    openset.erase(y_it);
                    new_y.g_score = tentative_g_score;
                    new_y.f_score = new_y.g_score + new_y.h_score;
                    openset.insert(new_y);
                    tentative_is_better = true;
                }

                if (tentative_is_better)
                {
                    auto came_it = came_from.find(y.cord);
                    if (came_it != came_from.end()) {
                        came_it->second = x.cord;
                    }
                    else came_from.insert(std::make_pair(y.cord, x.cord));
                }
            }
        }
        return {};
    }

} // namespace astar
} // namespace utils


#endif //RAG3_ENGINE_UTILS_ASTAR_H
