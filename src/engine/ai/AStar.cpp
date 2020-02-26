//
// Created by jul3x on 29.01.2020.
//


#include <engine/ai/AStar.h>
#include <engine/utils/Utils.h>


namespace r3e {
    namespace ai {
        bool operator==(const AStar::Node& a, const AStar::Node& b)
        {
            return a.cord.first == b.cord.first && a.cord.second == b.cord.second;
        }

        float AStar::heuristic(const Node& start, const Node& goal)
        {
            return std::hypot(start.cord.first - goal.cord.first, start.cord.second - goal.cord.second);
        }

        std::vector<AStar::Node>
        AStar::reconstructPath(const std::unordered_map<Node, Node, NodeHash>& came_from, const Node& current_node)
        {
            auto came_from_it = came_from.find(current_node);
            if (came_from_it != came_from.end())
            {
                std::vector<Node> p = reconstructPath(came_from, came_from_it->second);
                p.push_back(current_node);
                return p;
            }
            else
            {
                return {};
            }
        }

        ai::Path
        AStar::getSmoothedPath(const MapBlockage& map_blockage_, const sf::Vector2f& start, const sf::Vector2f& goal,
                               const NeighbourFunction& func, size_t limit)
        {
            int start_x = std::round(start.x / map_blockage_.scale_x_);
            int start_y = std::round(start.y / map_blockage_.scale_y_);
            int goal_x = std::round(goal.x / map_blockage_.scale_x_);
            int goal_y = std::round(goal.y / map_blockage_.scale_y_);

            auto convert = [&map_blockage_](const Node& node) {
                return sf::Vector2f(node.cord.first * map_blockage_.scale_x_,
                                    node.cord.second * map_blockage_.scale_y_);
            };

            std::vector<ai::AStar::Node> path = ai::AStar::getPath(map_blockage_.blockage_,
                                                                   sf::Vector2<size_t>(start_x, start_y),
                                                                   sf::Vector2<size_t>(goal_x, goal_y),
                                                                   func, limit);
            ai::Path ret;

            for (const auto& node : path)
            {
                ret.emplace_back(std::make_pair<sf::Vector2f, float>(convert(node), 0.0f));
            }
            AStar::getSmoothedPath_(ret);
            AStar::getSmoothedPath_(ret);

            // generate distance to goal on path
            for (auto it = ++ret.rbegin(), prev = ret.rbegin(); it != ret.rend(); ++it)
            {
                it->second = utils::geo::getDistance(it->first, prev->first) + prev->second;
                prev = it;
            }

            return ret;
        }

        void AStar::getSmoothedPath_(ai::Path& path)
        {
            if (path.size() < 3) return;
            sf::Vector2f curPoint, nextPoint;
            sf::Vector2f pointQ, pointR;

            for (auto it = path.begin(); it != path.end();)
            {
                curPoint = it->first;
                auto next_it = std::next(it);
                if (next_it == path.end()) break;
                nextPoint = next_it->first;
                pointQ = (0.75f * curPoint) +
                         (0.25f * nextPoint);
                pointR = (0.25f * curPoint) +
                         (0.75f * nextPoint);

                it->first = pointQ;
                path.insert(next_it, std::make_pair(pointR, 0.0f));
                it = next_it;
            }
        }

        std::vector<AStar::Node>
        AStar::getPath(const std::vector<std::vector<bool>>& grid, const sf::Vector2<size_t>& start,
                       const sf::Vector2<size_t>& goal, const NeighbourFunction& func, size_t limit)
        {
            std::unordered_set<Node, NodeHash> closed_set;
            std::unordered_set<Node, NodeHash> open_set;

            static constexpr float INF = std::numeric_limits<float>::infinity();

            open_set.insert(Node({start.x, start.y}, 0.0f, 0.0f, 0.0f));
            Node goal_node = Node({goal.x, goal.y}, INF, INF, INF);

            std::unordered_map<Node, Node, NodeHash> came_from;

            while (!open_set.empty())
            {
                auto x_it = open_set.begin();

                for (auto it = open_set.begin(); it != open_set.end(); ++it)
                {
                    if (it->f_score < x_it->f_score) x_it = it;
                }

                Node x = *x_it;
                open_set.erase(x_it);

                if (x == goal_node)
                {
                    return reconstructPath(came_from, goal_node);
                }

                closed_set.insert(x);

                auto neighbours = func(grid, sf::Vector2<size_t>(x.cord.first, x.cord.second));

                for (const auto& neigh : neighbours)
                {
                    --limit;
                    if (limit <= 0) return {};

                    Node y = Node({neigh.x, neigh.y}, INF, INF, INF);
                    if (closed_set.find(y) != closed_set.end())
                    {
                        continue;
                    }

                    float tentative_g_score = x.g_score +
                                              (neigh.x != x.cord.first && neigh.y != x.cord.second) ? 1.1f : 1.0f;
                    bool tentative_is_better = false;

                    auto y_it = open_set.find(y);

                    if (y_it == open_set.end())
                    {
                        y.h_score = heuristic(y, goal_node);
                        y.g_score = tentative_g_score;
                        y.f_score = y.g_score + y.h_score;
                        open_set.insert(y);
                        tentative_is_better = true;
                    }
                    else if (tentative_g_score < y_it->g_score)
                    {
                        Node new_y = Node({y_it->cord.first, y_it->cord.second}, y_it->f_score, y_it->g_score,
                                          y_it->h_score);
                        open_set.erase(y_it);
                        new_y.g_score = tentative_g_score;
                        new_y.f_score = new_y.g_score + new_y.h_score;
                        open_set.insert(new_y);
                        tentative_is_better = true;
                    }

                    if (tentative_is_better)
                    {
                        auto came_it = came_from.find(y);
                        if (came_it != came_from.end())
                        {
                            came_it->second = x;
                        }
                        else came_from.insert(std::make_pair(y, x));
                    }
                }
            }
            return {};
        }

        NeighboursVec AStar::EightNeighbours(const Grid& grid, const sf::Vector2<size_t>& pos)
        {
            static std::vector<sf::Vector2i> neighbours = {
                    {-1, 0},
                    {0,  -1},
                    {0,  1},
                    {1,  0}
            };

            static std::vector<sf::Vector2i> diag_neighbours = {
                    {-1, -1},
                    {1,  -1},
                    {1,  1},
                    {-1, 1}
            };

            NeighboursVec ret;
            for (const auto& neigh : neighbours)
            {
                int cord_x = pos.x + neigh.x;
                int cord_y = pos.y + neigh.y;

                if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

                if (grid[cord_x][cord_y]) continue;

                ret.emplace_back(cord_x, cord_y);
            }

            for (const auto& neigh : diag_neighbours)
            {
                int cord_x = pos.x + neigh.x;
                int cord_y = pos.y + neigh.y;

                if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

                if (grid[cord_x][cord_y]) continue;

                if (grid[pos.x][cord_y] || grid[cord_x][pos.y]) continue;

                ret.emplace_back(cord_x, cord_y);
            }

            return ret;
        }

        NeighboursVec AStar::FourNeighbours(const Grid& grid, const sf::Vector2<size_t>& pos)
        {
            static std::vector<sf::Vector2i> neighbours = {
                    {-1, 0},
                    {0,  -1},
                    {0,  1},
                    {1,  0}
            };

            NeighboursVec ret;
            for (const auto& neigh : neighbours)
            {
                int cord_x = pos.x + neigh.x;
                int cord_y = pos.y + neigh.y;

                if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

                if (grid[cord_x][cord_y]) continue;

                ret.emplace_back(cord_x, cord_y);
            }

            return ret;
        }
    } // namespace ai
} // namespace r3e