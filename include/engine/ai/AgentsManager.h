//
// Created by jul3x on 04.02.2020.
//

#ifndef RAG3_ENGINE_AI_AGENTSMANAGER_H
#define RAG3_ENGINE_AI_AGENTSMANAGER_H

#include <chrono>
#include <queue>
#include <unordered_map>

#include <SFML/System/Vector2.hpp>

#include <engine/ai/AbstractAgent.h>
#include <engine/ai/AStar.h>


namespace ai {

    class AgentsManager {
    public:
        using Path = std::list<sf::Vector2f>;
        using Goal = sf::Vector2f;
        using Timestamp = std::chrono::system_clock::time_point;

        AgentsManager() = delete;

        AgentsManager(const ai::MapBlockage& map_blockage, ai::NeighbourFunction func,
                      float max_time_without_ms_ = 0.0f);

        void update();

        const Path& getPath(const AbstractAgent* agent) const;

        void setCurrentGoal(AbstractAgent* agent, const Goal& new_goal);

        void setNoGoal(AbstractAgent* agent);

        void registerAgent(AbstractAgent* agent);

        void deleteAgent(AbstractAgent* agent);

        const Goal& getCurrentGoal(const AbstractAgent* agent) const;

    private:
        inline std::tuple<Path, Goal, Timestamp>& getAgentData(AbstractAgent* agent);

        inline const std::tuple<Path, Goal, Timestamp>& getAgentData(AbstractAgent* agent) const;

        static inline bool isGoalValid(const Goal& goal);

        float max_time_without_ms_;
        const ai::MapBlockage& map_blockage_;
        ai::NeighbourFunction neighbour_function_;

        std::unordered_map<AbstractAgent*, std::tuple<Path, Goal, Timestamp>> agents_map_;

        std::queue<AbstractAgent*> agents_to_update_;
    };
} // namespace ai


#endif //RAG3_ENGINE_AI_AGENTSMANAGER_H
