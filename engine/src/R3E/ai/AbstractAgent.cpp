//
// Created by jul3x on 04.02.2020.
//

#include <R3E/ai/AbstractAgent.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>


namespace r3e {
    namespace ai {
        AbstractAgent::AbstractAgent() : manager_(), can_avoid_specials_(true)
        {
        }

        void AbstractAgent::registerAgentsManager(std::shared_ptr<AgentsManager> manager)
        {
            manager_ = std::move(manager);
            manager_->registerAgent(this);

            this->setNoGoal();
        }

        AbstractAgent::~AbstractAgent()
        {
            this->unregisterAgentsManager();
        }

        void AbstractAgent::setCurrentGoal(const sf::Vector2f& goal)
        {
            manager_->setCurrentGoal(this, goal);
        }

        void AbstractAgent::setNoGoal()
        {
            manager_->setNoGoal(this);
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

                if (change_direction == 1)
                    wandering_direction_ = -wandering_direction_;
            }

            return wandering_direction_;
        }

        const Path& AbstractAgent::getPath() const
        {
            return manager_->getPath(this);
        }

        const sf::Vector2f& AbstractAgent::getCurrentGoal() const
        {
            return manager_->getCurrentGoal(this);
        }

        sf::Vector2f AbstractAgent::generateVelocityForPath() const
        {
            const sf::Vector2f& pos = this->getStartPosition();
            const ai::Path& path = manager_->getPath(this);
            if (path.empty())
                return {};

            sf::Vector2f point = utils::geo::getNearestForwardPointToPath(pos, path);

            return utils::geo::getNormalized(point - pos);
        }

        void AbstractAgent::setCanAvoidSpecials(bool can)
        {
            can_avoid_specials_ = can;
        }

        bool AbstractAgent::canAvoidSpecials() const
        {
            return can_avoid_specials_;
        }

        void AbstractAgent::unregisterAgentsManager()
        {
            if (manager_ != nullptr)
                manager_->deleteAgent(this);
        }

    } // namespace ai
} // namespace r3e