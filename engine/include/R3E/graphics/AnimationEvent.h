//
// Created by jul3x on 05.09.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_ANIMATIONEVENT_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_ANIMATIONEVENT_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LightPoint.h>

namespace r3e::graphics {

    class AnimationEvent : public AbstractDrawableObject {

    public:
        enum class AnimationType {
            Linear,
            Quadratic
        };

        AnimationEvent() = delete;

        AnimationEvent(sf::Texture* texture,
                       const sf::Vector2i& frame_size,
                       const sf::Vector2f& position,
                       int z_index,
                       float duration_s, short int frames_count,
                       const AnimationType& animation_type = AnimationType::Linear);

        bool update(float time_elapsed);

        graphics::LightPoint* getLightPoint() const;

    protected:
        short int max_frames_count_;
        float duration_s_;

        sf::Vector2i frame_size_;

        sf::IntRect animation_source_;

        AnimationType type_;

        float time_elapsed_;

        std::unique_ptr<graphics::LightPoint> light_;

    };

} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_ANIMATIONEVENT_H
