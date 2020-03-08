//
// Created by jul3x on 04.02.2020.
//

#include <engine/ai/AbstractAgent.h>
#include <engine/ai/AgentsManager.h>
#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>


namespace r3e {
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

        const sf::Vector2f&
        AbstractAgent::getWanderingDirection(float constraint, float max_time_ms, int when_opposite)
        {
            auto now = std::chrono::system_clock::now();

            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - latest_wander_point_time_).count() >
                max_time_ms)
            {
                auto new_direction = std::get<1>(utils::geo::cartesianToPolar(wandering_direction_)) +
                                     utils::num::getRandom(-constraint, constraint);
                wandering_direction_ = {std::cos(new_direction), std::sin(new_direction)};
                latest_wander_point_time_ = now;

                auto change_direction = utils::num::getRandom(0, when_opposite);

                if (change_direction == 1) wandering_direction_ = -wandering_direction_;
            }

            return wandering_direction_;
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
} // namespace r3e