//
// Created by jul3x on 29.01.2020.
//


#include <engine/ai/AStar.h>
#include <engine/utils/Utils.h>


bool operator== (const AStar::Node &a, const AStar::Node &b) {
    return a.cord.first == b.cord.first && a.cord.second == b.cord.second;
}

float AStar::heuristic(const Node &start, const Node &goal)
{
    return std::hypot(start.cord.first - goal.cord.first, start.cord.second - goal.cord.second);
}

std::vector<AStar::Node> AStar::reconstructPath(const std::unordered_map<Node, Node, NodeHash> &came_from, const Node &current_node) {
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

std::vector<sf::Vector2f> AStar::getSmoothedPath(const std::vector<AStar::Node> &path, float scale_x, float scale_y) {
    if (path.empty()) return {};

    std::vector<sf::Vector2f> ret;

    auto convert = [&scale_x, &scale_y](const Node &node) {
        return sf::Vector2f(node.cord.first * scale_x, node.cord.second * scale_y);
    };

    ret.push_back(convert(path[0]));

    sf::Vector2f curPoint, prevPoint, nextPoint;
    sf::Vector2f currentHeading, nextHeading, pointQ, pointR;

    float angle;

    prevPoint = convert(path[0]);
    int i = 1;

    while(i < path.size() - 1)
    {
        curPoint = convert(path[i++]);
        nextPoint = convert(path[i]);
        currentHeading = utils::geo::getNormalized(curPoint - prevPoint);
        nextHeading = utils::geo::getNormalized(nextPoint - curPoint);
        angle = utils::geo::getAngle(currentHeading, nextHeading);
//        if (angle >= M_PI_2)
//        {
//            i++;
//            prevPoint = curPoint;
//            continue;
//        }
        pointQ = (0.75f * curPoint) +
                 (0.25f * nextPoint);
        pointR = (0.25f * curPoint) +
                 (0.75f * nextPoint);
        ret.push_back(pointQ);
        ret.push_back(pointR);
        prevPoint = pointR;
    }

    // Make sure we get home.
    if(ret.back() != convert(path.back()))
        ret.push_back(convert(path.back()));

    return ret;

}

std::vector<AStar::Node> AStar::makePath(const std::vector<std::vector<bool>> &grid, const sf::Vector2<size_t> &start, const sf::Vector2<size_t> &goal)
{
    // TODO REIMPLEMENT EVERYTHING!

    std::unordered_set<Node, NodeHash> closedset;
    std::unordered_set<Node, NodeHash> openset;

    static constexpr float INF = std::numeric_limits<float>::infinity();

    openset.insert(Node({start.x, start.y}, 0.0f, 0.0f, 0.0f));
    Node goal_node = Node({goal.x, goal.y}, INF, INF, INF);

    static std::vector<std::pair<int, int>> neighbours = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    int i = 0;
    std::unordered_map<Node, Node, NodeHash> came_from;

    while (!openset.empty())
    {
        auto x_it = openset.begin();

        for (auto it = openset.begin(); it != openset.end(); ++it)
        {
            if (it->f_score < x_it->f_score) x_it = it;
        }
        Node x = *x_it;
        openset.erase(x_it);

        if (x == goal_node) {
            return reconstructPath(came_from, goal_node);
        }

        closedset.insert(x);

        for (const auto &neigh : neighbours)
        {
            int cord_x = x.cord.first + neigh.first;
            int cord_y = x.cord.second + neigh.second;

            if (cord_x < 0 || cord_x >= grid.size() || cord_y < 0 || cord_y >= grid.at(0).size()) continue;

            if (grid[cord_x][cord_y]) continue;

            Node y = Node({cord_x, cord_y}, INF, INF, INF);
            if (closedset.find(y) != closedset.end()) {
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
                auto came_it = came_from.find(y);
                if (came_it != came_from.end()) {
                    came_it->second = x;
                }
                else came_from.insert(std::make_pair(y, x));
            }
        }
    }
    return {};
}
