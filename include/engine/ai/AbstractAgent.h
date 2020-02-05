//
// Created by jul3x on 04.02.2020.
//

#ifndef RAG3_ENGINE_AI_ABSTRACTAGENT_H
#define RAG3_ENGINE_AI_ABSTRACTAGENT_H

#include <SFML/System/Vector2.hpp>

#include <list>

namespace ai {
    class AgentsManager;

    class AbstractAgent {
    public:
        explicit AbstractAgent(AgentsManager& manager);

        virtual ~AbstractAgent();

        void setCurrentGoal(const sf::Vector2f& goal);

        void setNoGoal();

        const sf::Vector2f& getCurrentGoal() const;

        virtual const sf::Vector2f& getStartPosition() const = 0;

        const std::list<sf::Vector2f>& getPath() const;

    private:
        AgentsManager& manager_;

    };
} // namespace ai

#endif //RAG3_ENGINE_AI_ABSTRACTAGENT_H
