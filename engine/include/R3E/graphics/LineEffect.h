//
// Created by jul3x on 20.10.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LINEEFFECT_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LINEEFFECT_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/StateWithInertia.h>
#include <R3E/utils/Geometry.h>


namespace r3e::graphics {
    class LineEffect : public sf::Drawable {

    public:
        LineEffect(const sf::Color& color, float change_speed) : start_p_(change_speed), end_p_(change_speed), line_(sf::TriangleStrip, 4)
        {
            line_[0].color = color;
            line_[1].color = color;
            line_[2].color = color;
            line_[3].color = color;
        }

        [[nodiscard]] const sf::Vector2f getStart() const
        {
            return start_p_.getState();
        }

        [[nodiscard]] const sf::Vector2f getEnd() const
        {
            return end_p_.getState();
        }

        void setStart(const sf::Vector2f& start_p)
        {
            start_p_.setState(start_p);
        }

        void setEnd(const sf::Vector2f& end_p)
        {
            end_p_.setState(end_p);
        }

        void setColor(const sf::Color& color)
        {
            line_[0].color = color;
            line_[1].color = color;
            line_[2].color = color;
            line_[3].color = color;
        }

        void forceSet()
        {
            start_p_.setForcedState(start_p_.getSetState());
            end_p_.setForcedState(end_p_.getSetState());
        }

        void setThickness(float thickness)
        {
            thickness_ = thickness;
        }

        void update(float time_elapsed)
        {
            start_p_.update(time_elapsed);
            end_p_.update(time_elapsed);

            auto unit_direction = utils::geo::getNormalized(end_p_.getState() - start_p_.getState());
            auto unit_perpendicular = sf::Vector2f(- unit_direction.y, unit_direction.x);

            auto offset = (thickness_ / 2.0f) * unit_perpendicular;

            line_[0].position = start_p_.getState() + offset;
            line_[1].position = end_p_.getState() + offset;
            line_[2].position = start_p_.getState() - offset;
            line_[3].position = end_p_.getState() - offset;
        }

    protected:
        StateWithInertia<sf::Vector2f> start_p_, end_p_;
        float thickness_{2.0f};
        sf::VertexArray line_;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(line_, states);
        }

    };
}

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LINEEFFECT_H
