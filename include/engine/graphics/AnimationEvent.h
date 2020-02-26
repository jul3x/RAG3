//
// Created by jul3x on 05.09.19.
//

#ifndef RAG3_ENGINE_GRAPHICS_ANIMATIONEVENT_H
#define RAG3_ENGINE_GRAPHICS_ANIMATIONEVENT_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <engine/objects/AbstractDrawableObject.h>


namespace r3e {
    namespace graphics {

        class AnimationEvent : public AbstractDrawableObject {

        public:
            enum class AnimationType {
                LINEAR,
                QUADRATIC
            };

            AnimationEvent() = delete;

            AnimationEvent(sf::Texture* texture,
                           const sf::Vector2i& frame_size,
                           const sf::Vector2f& position,
                           float duration_s, short int frames_count,
                           const AnimationType& animation_type = AnimationType::LINEAR);

            bool update(float time_elapsed);

        protected:
            short int max_frames_count_;
            float duration_s_;

            sf::Vector2i frame_size_;

            sf::IntRect animation_source_;

            AnimationType type_;

            float time_elapsed_;

        };

    } // namespace graphics
} // namespace r3e

#endif // RAG3_ENGINE_GRAPHICS_ANIMATIONEVENT_H
