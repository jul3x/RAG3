//
// Created by jul3x on 08.04.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_STATEWITHINERTIA_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_STATEWITHINERTIA_H

namespace r3e {

    template<class T>
    class StateWithInertia {
    public:
        explicit StateWithInertia(float change_speed) : change_speed_(change_speed), set_(), current_() {}

        explicit StateWithInertia() : StateWithInertia(1.0f) {}

        [[nodiscard]] float getChangeSpeed() const
        {
            return change_speed_;
        }

        [[nodiscard]] const T& getState() const
        {
            return current_;
        }

        [[nodiscard]] const T& getSetState() const
        {
            return set_;
        }

        void setChangeSpeed(float speed)
        {
            change_speed_ = speed;
        }

        void setState(const T& state)
        {
            set_ = state;
        }

        void setForcedState(const T& state)
        {
            set_ = state;
            current_ = state;
        }

        void update(float time_elapsed)
        {
            auto diff = set_ - current_;
            current_ = current_ + diff * change_speed_ * time_elapsed;
        }

    private:
        T current_, set_;
        float change_speed_;

    };

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_STATEWITHINERTIA_H
