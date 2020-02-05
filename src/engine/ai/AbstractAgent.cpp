//
// Created by jul3x on 04.02.2020.
//

#include <engine/ai/AbstractAgent.h>
#include <engine/ai/AgentsManager.h>


namespace ai {
    AbstractAgent::AbstractAgent(AgentsManager& manager) : manager_(manager)
    {
        manager_.registerAgent(this);
    }

    AbstractAgent::~AbstractAgent()
    {
        manager_.deleteAgent(this);
    }

    void AbstractAgent::setCurrentGoal(const sf::Vector2f& goal)
    {
        manager_.setCurrentGoal(this, goal);
    }

    void AbstractAgent::setNoGoal()
    {
        manager_.setNoGoal(this);
    }

    const std::list<sf::Vector2f>& AbstractAgent::getPath() const
    {
        return manager_.getPath(this);
    }

    const sf::Vector2f& AbstractAgent::getCurrentGoal() const
    {
        return manager_.getCurrentGoal(this);
    }
} // namespace ai