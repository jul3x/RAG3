//
// Created by jul3x on 04.02.2020.
//

#ifndef RAG3_ENGINE_AI_AGENTSMANAGER_H
#define RAG3_ENGINE_AI_AGENTSMANAGER_H

#include <chrono>
#include <queue>
#include <unordered_map>

#include <SFML/System/Vector2.hpp>

#include <R3E/ai/AbstractAgent.h>
#include <R3E/ai/AStar.h>
#include <R3E/ai/DataTypes.h>


namespace r3e {
    namespace ai {

        class AgentsManager {
        public:
            AgentsManager() = delete;

            AgentsManager(const ai::MapBlockage& map_blockage, ai::NeighbourFunction func,
                          float max_time_without_ms_ = 0.0f, float min_threshold_goal = 1.0f,
                          size_t max_path_search_limit = 1e12);

            void update();

            const ai::Path& getPath(const AbstractAgent* agent) const;

            void setCurrentGoal(AbstractAgent* agent, const ai::Goal& new_goal);

            void setNoGoal(AbstractAgent* agent);

            void registerAgent(AbstractAgent* agent);

            void deleteAgent(AbstractAgent* agent);

            const Goal& getCurrentGoal(const AbstractAgent* agent) const;

        private:
            inline std::tuple<ai::Path, ai::Goal, ai::Timestamp>& getAgentData(AbstractAgent* agent);

            inline const std::tuple<ai::Path, ai::Goal, ai::Timestamp>& getAgentData(AbstractAgent* agent) const;

            static inline bool isGoalValid(const Goal& goal);

            float max_time_without_ms_, min_threshold_goal_;
            size_t max_path_search_limit_;
            const ai::MapBlockage& map_blockage_;
            ai::NeighbourFunction neighbour_function_;

            std::unordered_map<AbstractAgent*, std::tuple<ai::Path, ai::Goal, ai::Timestamp>> agents_map_;

            std::queue<AbstractAgent*> agents_to_update_;
            std::unordered_set<AbstractAgent*> agents_to_update_set_;
        };
    } // namespace ai
} // namespace r3e

#endif //RAG3_ENGINE_AI_AGENTSMANAGER_H