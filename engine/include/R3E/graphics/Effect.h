//
// Created by jul3x on 23.04.20.
//

#ifndef RAG3_ENGINE_GRAPHICS_EFFECT_H
#define RAG3_ENGINE_GRAPHICS_EFFECT_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "R3E/objects/AbstractDrawableObject.h"


namespace r3e {
    namespace graphics {

        class Effect : public AbstractDrawableObject {

        public:
            Effect(const sf::Vector2f& size, float duration);

            virtual bool update(float time_elapsed);

        protected:
            float duration_s_;

            float time_elapsed_;

        };


        class FadeInOut : public Effect {

        public:
            FadeInOut(const sf::Vector2f& size, const sf::Color& color, float duration);

            bool update(float time_elapsed) override;

        private:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

            sf::RectangleShape effect_;
            sf::Color color_;

        };

    } // namespace graphics
} // namespace r3e

#endif // RAG3_ENGINE_GRAPHICS_EFFECT_H
