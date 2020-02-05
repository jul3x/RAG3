//
// Created by jul3x on 04.02.2020.
//


#include <utility>
#include <iostream>

#include <engine/ai/AgentsManager.h>
#include <engine/ai/AStar.h>


namespace ai {
    AgentsManager::AgentsManager(const ai::MapBlockage& map_blockage, ai::NeighbourFunction  func,
                                 float max_time_without_ms) : max_time_without_ms_(max_time_without_ms),
                                                              map_blockage_(map_blockage),
                                                              neighbour_function_(std::move(func))
    {

    }

    void AgentsManager::update()
    {
        auto time_now = std::chrono::system_clock::now();

        for (const auto& agent : agents_map_)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(time_now - std::get<2>(agent.second)).count() >
                max_time_without_ms_ && AgentsManager::isGoalValid(std::get<1>(agent.second)))
                agents_to_update_.push(agent.first);
        }

        if (agents_to_update_.empty()) return;

        std::cout << "[AgentsManager] Calculating new path!" << std::endl;
        auto& agent = agents_to_update_.front();

        try
        {
            auto& data = AgentsManager::getAgentData(agent);
            std::get<0>(data) = ai::AStar::getSmoothedPath(map_blockage_, agent->getStartPosition(),
                                                           std::get<1>(data), neighbour_function_);
            std::get<2>(data) = std::chrono::system_clock::now();
        }
        catch (const std::runtime_error& e)
        {
            std::cout << "[AgentsManager] Agent removed while processing the path!" << std::endl;
        }

        agents_to_update_.pop();
    }

    const AgentsManager::Path& AgentsManager::getPath(const AbstractAgent* agent) const
    {
        return std::get<0>(this->getAgentData(const_cast<AbstractAgent*>(agent)));
    }

    void AgentsManager::setCurrentGoal(AbstractAgent* agent, const AgentsManager::Goal& new_goal)
    {
        std::get<1>(this->getAgentData(agent)) = new_goal;
        agents_to_update_.push(agent);
        std::cout << "[AgentsManager] Goal set to " << new_goal.x << ", " << new_goal.y << "!" << std::endl;
    }

    void AgentsManager::setNoGoal(AbstractAgent* agent)
    {
        std::get<1>(this->getAgentData(agent)) = sf::Vector2f{std::numeric_limits<float>::infinity(),
                                                              std::numeric_limits<float>::infinity()};
        std::cout << "[AgentsManager] Goal set to 0!" << std::endl;
    }

    void AgentsManager::registerAgent(AbstractAgent* agent)
    {
        agents_map_.emplace(agent, std::tuple<AgentsManager::Path, AgentsManager::Goal, AgentsManager::Timestamp>());
        std::cout << "[AgentsManager] Agent registered!" << std::endl;
    }

    void AgentsManager::deleteAgent(AbstractAgent* agent)
    {
        auto it = agents_map_.find(agent);

        if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent to delete not found!");
        agents_map_.erase(agent);
        std::cout << "[AgentsManager] Agent deleted!" << std::endl;
    }

    const AgentsManager::Goal& AgentsManager::getCurrentGoal(const AbstractAgent* agent) const
    {
        return std::get<1>(this->getAgentData(const_cast<AbstractAgent*>(agent)));
    }

    std::tuple<AgentsManager::Path, AgentsManager::Goal, AgentsManager::Timestamp>&
    AgentsManager::getAgentData(AbstractAgent* agent)
    {
        auto it = agents_map_.find(agent);

        if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent not found!");
        return it->second;
    }

    const std::tuple<AgentsManager::Path, AgentsManager::Goal, AgentsManager::Timestamp>&
    AgentsManager::getAgentData(AbstractAgent* agent) const
    {
        auto it = agents_map_.find(agent);

        if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent not found!");
        return it->second;
    }

    bool AgentsManager::isGoalValid(const AgentsManager::Goal& goal)
    {
        return !(goal.x == std::numeric_limits<float>::infinity() || goal.y == std::numeric_limits<float>::infinity());
    }

} // namespace ai