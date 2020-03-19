//
// Created by jul3x on 05.09.19.
//

#include <cmath>
#include <stdexcept>

#include <R3E/graphics/AnimationEvent.h>


namespace r3e {
    namespace graphics {

        AnimationEvent::AnimationEvent(sf::Texture* texture,
                                       const sf::Vector2i& frame_size,
                                       const sf::Vector2f& position,
                                       float duration_s, short int frames_count,
                                       const AnimationType& animation_type) :
                AbstractDrawableObject(position,
                                       static_cast<sf::Vector2f>(frame_size),
                                       texture),
                frame_size_(frame_size),
                animation_source_({0, 0}, frame_size_),
                type_(animation_type),
                duration_s_(duration_s),
                max_frames_count_(frames_count),
                time_elapsed_(0.0f)
        {
            shape_.setTextureRect(animation_source_);
        }

        bool AnimationEvent::update(float time_elapsed)
        {
            time_elapsed_ += time_elapsed;

            auto current_frame = static_cast<short int>(time_elapsed_ * max_frames_count_ / duration_s_);

            if (current_frame >= max_frames_count_)
            {
                return true;
            }

            switch (type_)
            {
                case AnimationType::LINEAR:
                {
                    animation_source_.left = frame_size_.x * current_frame;
                    animation_source_.top = 0;
                    break;
                }
                case AnimationType::QUADRATIC:
                {
                    animation_source_.left = frame_size_.x *
                                             (current_frame % static_cast<short int>(std::sqrt(max_frames_count_)));
                    animation_source_.top = frame_size_.y *
                                            (current_frame / static_cast<short int>(std::sqrt(max_frames_count_)));
                    break;
                }
                default:
                {
                    throw std::invalid_argument("[Animation] Animation type not implemented!");
                }
            }

            shape_.setTextureRect(animation_source_);

            return false;
        }

    } // namespace graphics
} // namespace r3e