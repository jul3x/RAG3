//
// Created by jul3x on 23.04.20.
//

#include <R3E/graphics/Effect.h>
#include <iostream>


namespace r3e {
    namespace graphics {

        Effect::Effect(const sf::Vector2f& size, float duration) :
                duration_s_(duration), time_elapsed_(duration),
                AbstractDrawableObject({0.0f, 0.0f}, size, nullptr)
        {

        }

        bool Effect::update(float time_elapsed)
        {
            time_elapsed_ -= time_elapsed;

            return time_elapsed_ < 0.0f;
        }

        FadeInOut::FadeInOut(const sf::Vector2f& size, const sf::Color& color, float duration) :
                Effect(size, duration),
                color_(color.r, color.g, color.b, 0),
                effect_(size)
        {
            effect_.setPosition(0.0f, 0.0f);
        }

        bool FadeInOut::update(float time_elapsed)
        {
            auto delta = static_cast<int>(time_elapsed * 512.0f / duration_s_);
            if (2.0f * time_elapsed_ < duration_s_)
                delta = -delta;

            color_.a = static_cast<sf::Uint8>(std::min(std::max(color_.a + delta, 0), 255));

            effect_.setFillColor(color_);

            return Effect::update(time_elapsed);
        }

        void FadeInOut::draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            target.draw(effect_, states);
        }

    } // namespace graphics
} // namespace r3e