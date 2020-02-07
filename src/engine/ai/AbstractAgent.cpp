//
// Created by jul3x on 04.02.2020.
//

#include <engine/ai/AbstractAgent.h>
#include <engine/ai/AgentsManager.h>
#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>


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

    const Path& AbstractAgent::getPath() const
    {
        return manager_.getPath(this);
    }

    const sf::Vector2f& AbstractAgent::getCurrentGoal() const
    {
        return manager_.getCurrentGoal(this);
    }

    sf::Vector2f AbstractAgent::generateVelocityForPath() const
    {
        const sf::Vector2f& pos = this->getStartPosition();
        const ai::Path& path = manager_.getPath(this);
        if (path.empty()) return {};

        sf::Vector2f point = utils::geo::getNearestForwardPointToPath(pos, path);

        return utils::geo::getNormalized(point - pos);
    }
} // namespace ai