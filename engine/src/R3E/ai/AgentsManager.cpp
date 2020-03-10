//
// Created by jul3x on 04.02.2020.
//


#include <utility>
#include <iostream>

#include <R3E/ai/AgentsManager.h>
#include <R3E/ai/AStar.h>
#include <R3E/utils/Numeric.h>


namespace r3e {
    namespace ai {
        AgentsManager::AgentsManager(const ai::MapBlockage& map_blockage, ai::NeighbourFunction func,
                                     float max_time_without_ms, float min_threshold_goal, size_t max_path_search_limit)
                :
                max_time_without_ms_(max_time_without_ms),
                min_threshold_goal_(min_threshold_goal),
                map_blockage_(map_blockage),
                neighbour_function_(std::move(func)),
                max_path_search_limit_(max_path_search_limit)
        {
            if (max_time_without_ms_ < 0.0f || min_threshold_goal_ < 1.0f)
            {
                throw std::invalid_argument("Wrong parameters in AgentsManager constructor!");
            }
        }

        void AgentsManager::update()
        {
            auto time_now = std::chrono::system_clock::now();

            for (const auto& agent : agents_map_)
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                        time_now - std::get<2>(agent.second)).count() >
                    max_time_without_ms_ && AgentsManager::isGoalValid(std::get<1>(agent.second)) &&
                    agents_to_update_set_.find(agent.first) == agents_to_update_set_.end())
                {
                    agents_to_update_.push(agent.first);
                    agents_to_update_set_.insert(agent.first);
                }
            }

            if (agents_to_update_.empty()) return;

            //std::cout << "[AgentsManager] Calculating new path!" << std::endl;
            auto& agent = agents_to_update_.front();

            try
            {
                auto& data = AgentsManager::getAgentData(agent);
                std::get<0>(data) = ai::AStar::getSmoothedPath(map_blockage_, agent->getStartPosition(),
                                                               std::get<1>(data), neighbour_function_,
                                                               max_path_search_limit_);
                std::get<2>(data) = std::chrono::system_clock::now();
            }
            catch (const std::runtime_error& e)
            {
                std::cout << "[AgentsManager] Agent removed while processing the path!" << std::endl;
            }

            agents_to_update_set_.erase(agents_to_update_set_.find(agent));
            agents_to_update_.pop();
        }

        const ai::Path& AgentsManager::getPath(const AbstractAgent* agent) const
        {
            return std::get<0>(this->getAgentData(const_cast<AbstractAgent*>(agent)));
        }

        void AgentsManager::setCurrentGoal(AbstractAgent* agent, const ai::Goal& new_goal)
        {
            if (!utils::num::isNearlyEqual(this->getCurrentGoal(agent), new_goal, min_threshold_goal_))
            {
                if (agents_to_update_set_.find(agent) == agents_to_update_set_.end())
                {
                    agents_to_update_set_.insert(agent);
                    agents_to_update_.push(agent);
                }

                std::get<1>(this->getAgentData(agent)) = new_goal;
                //std::cout << "[AgentsManager] Goal set to " << new_goal.x << ", " << new_goal.y << "!" << std::endl;
            }
        }

        void AgentsManager::setNoGoal(AbstractAgent* agent)
        {
            std::get<1>(this->getAgentData(agent)) = {ai::NO_GOAL, ai::NO_GOAL};

            auto& data = AgentsManager::getAgentData(agent);
            std::get<0>(data) = {};
            //std::cout << "[AgentsManager] Goal set to 0!" << std::endl;
        }

        void AgentsManager::registerAgent(AbstractAgent* agent)
        {
            agents_map_.emplace(agent, std::tuple<ai::Path, ai::Goal, ai::Timestamp>());
            std::cout << "[AgentsManager] Agent registered!" << std::endl;
        }

        void AgentsManager::deleteAgent(AbstractAgent* agent)
        {
            auto it = agents_map_.find(agent);

            if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent to delete not found!");
            agents_map_.erase(agent);
            std::cout << "[AgentsManager] Agent deleted!" << std::endl;
        }

        const ai::Goal& AgentsManager::getCurrentGoal(const AbstractAgent* agent) const
        {
            return std::get<1>(this->getAgentData(const_cast<AbstractAgent*>(agent)));
        }

        std::tuple<ai::Path, ai::Goal, ai::Timestamp>&
        AgentsManager::getAgentData(AbstractAgent* agent)
        {
            auto it = agents_map_.find(agent);

            if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent not found!");
            return it->second;
        }

        const std::tuple<ai::Path, ai::Goal, ai::Timestamp>&
        AgentsManager::getAgentData(AbstractAgent* agent) const
        {
            auto it = agents_map_.find(agent);

            if (it == agents_map_.end()) throw std::runtime_error("[AgentsManager] Agent not found!");
            return it->second;
        }

        bool AgentsManager::isGoalValid(const ai::Goal& goal)
        {
            return goal.x != ai::NO_GOAL && goal.y != ai::NO_GOAL;
        }

    } // namespace ai
} // namespace r3e