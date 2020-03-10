//
// Created by jul3x on 04.02.2020.
//

#ifndef RAG3_ENGINE_AI_ABSTRACTAGENT_H
#define RAG3_ENGINE_AI_ABSTRACTAGENT_H

#include <chrono>

#include <SFML/System/Vector2.hpp>

#include <R3E/ai/DataTypes.h>


namespace r3e {
    namespace ai {

        class AgentsManager;

        class AbstractAgent {
        public:
            explicit AbstractAgent(AgentsManager& manager);

            virtual ~AbstractAgent();

            void setCurrentGoal(const ai::Goal& goal);

            void setNoGoal();

            const sf::Vector2f& getWanderingDirection(float constraint, float max_time_ms, int when_opposite);

            const ai::Goal& getCurrentGoal() const;

            virtual const sf::Vector2f& getStartPosition() const = 0;

            const ai::Path& getPath() const;

            sf::Vector2f generateVelocityForPath() const;

        private:
            AgentsManager& manager_;

            ai::Timestamp latest_wander_point_time_;
            sf::Vector2f wandering_direction_;

        };

    } // namespace ai
} // namespace r3e

#endif //RAG3_ENGINE_AI_ABSTRACTAGENT_H
