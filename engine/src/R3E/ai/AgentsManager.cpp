//
// Created by jul3x on 04.02.2020.
//


#include <utility>
#include <iostream>

#include <R3E/ai/AgentsManager.h>
#include <R3E/ai/AStar.h>
#include <R3E/utils/Numeric.h>
#include <R3E/system/Logger.h>


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
                LOG.info("[AgentsManager] Agent removed while processing the path!");
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
            }
        }

        void AgentsManager::setNoGoal(AbstractAgent* agent)
        {
            std::get<1>(this->getAgentData(agent)) = {ai::NO_GOAL_, ai::NO_GOAL_};

            auto& data = AgentsManager::getAgentData(agent);
            std::get<0>(data) = {};
        }

        void AgentsManager::registerAgent(AbstractAgent* agent)
        {
            agents_map_.emplace(agent, std::tuple<ai::Path, ai::Goal, ai::Timestamp>());
            LOG.info("[AgentsManager] Agent registered!");
        }

        void AgentsManager::deleteAgent(AbstractAgent* agent)
        {
            auto it = agents_map_.find(agent);

            if (it == agents_map_.end()) return;
            agents_map_.erase(agent);
            LOG.info("[AgentsManager] Agent deleted!");
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
            return goal.x != ai::NO_GOAL_ && goal.y != ai::NO_GOAL_;
        }

    } // namespace ai
} // namespace r3e