//
// Created by jul3x on 29.01.2020.
//


#include <engine/ai/AStar.h>
#include <engine/utils/Utils.h>


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

    std::list<sf::Vector2f>
    AStar::getSmoothedPath(const MapBlockage& map_blockage_, const sf::Vector2f& start, const sf::Vector2f& goal)
    {
        int start_x = std::round(start.x / map_blockage_.scale_x_);
        int start_y = std::round(start.y / map_blockage_.scale_y_);
        std::cout <<start.x << " " << start_x << std::endl;
        int goal_x = std::round(goal.x / map_blockage_.scale_x_);
        int goal_y = std::round(goal.y / map_blockage_.scale_y_);

        auto convert = [&map_blockage_](const Node& node) {
            return sf::Vector2f(node.cord.first * map_blockage_.scale_x_, node.cord.second * map_blockage_.scale_y_);
        };

        std::vector<ai::AStar::Node> path = ai::AStar::getPath(map_blockage_.blockage_,
                                                               sf::Vector2<size_t>(start_x, start_y),
                                                               sf::Vector2<size_t>(goal_x, goal_y));
        std::list<sf::Vector2f> ret;

        for (const auto& node : path)
        {
            ret.push_back(convert(node));
        }
        AStar::getSmoothedPath_(ret);
        AStar::getSmoothedPath_(ret);
        AStar::getSmoothedPath_(ret);
        AStar::getSmoothedPath_(ret);

        return ret;
    }

    void AStar::getSmoothedPath_(std::list<sf::Vector2f>& path)
    {
        if (path.size() < 3) return;
        sf::Vector2f curPoint, nextPoint;
        sf::Vector2f pointQ, pointR;

        for (auto it = path.begin(); it != path.end();)
        {
            curPoint = *it;
            auto next_it = std::next(it);
            if (next_it == path.end()) break;
            nextPoint = *next_it;
            pointQ = (0.75f * curPoint) +
                     (0.25f * nextPoint);
            pointR = (0.25f * curPoint) +
                     (0.75f * nextPoint);

            *it = pointQ;
            path.insert(next_it, pointR);
            it = next_it;
        }
    }

    std::vector<AStar::Node>
    AStar::getPath(const std::vector<std::vector<bool>>& grid, const sf::Vector2<size_t>& start,
                   const sf::Vector2<size_t>& goal)
    {
        // TODO REIMPLEMENT EVERYTHING!

        std::unordered_set<Node, NodeHash> closed_set;
        std::unordered_set<Node, NodeHash> open_set;

        static constexpr float INF = std::numeric_limits<float>::infinity();

        open_set.insert(Node({start.x, start.y}, 0.0f, 0.0f, 0.0f));
        Node goal_node = Node({goal.x, goal.y}, INF, INF, INF);

        static std::vector<std::pair<int, int>> neighbours = {{-1, -1},
                                                              {-1, 0},
                                                              {-1, 1},
                                                              {0,  -1},
                                                              {0,  1},
                                                              {1,  -1},
                                                              {1,  0},
                                                              {1,  1}
                                                              };
        int i = 0;
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

            for (const auto& neigh : neighbours)
            {
                int cord_x = x.cord.first + neigh.first;
                int cord_y = x.cord.second + neigh.second;

                if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

                if (grid[cord_x][cord_y]) continue;

                Node y = Node({cord_x, cord_y}, INF, INF, INF);
                if (closed_set.find(y) != closed_set.end())
                {
                    continue;
                }

                float tentative_g_score =
                        x.g_score + std::hypot(x.cord.first - y.cord.first, x.cord.second - y.cord.second);
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
}