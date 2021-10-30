//
// Created by jul3x on 05.09.19.
//

#include <cmath>
#include <stdexcept>

#include <R3E/graphics/AnimationEvent.h>


namespace r3e::graphics {

    AnimationEvent::AnimationEvent(sf::Texture* texture,
                                   const sf::Vector2i& frame_size,
                                   const sf::Vector2f& position,
                                   int z_index,
                                   float duration_s, short int frames_count,
                                   const AnimationType& animation_type) :
            AbstractDrawableObject(position,
                                   static_cast<sf::Vector2f>(frame_size),
                                   texture, z_index, frames_count, duration_s / static_cast<float>(frames_count)),
            type_(animation_type),
            duration_s_(duration_s),
            time_elapsed_(0.0f),
            speed_factor_(1.0f),
            reversed_(false)
    {
        shape_.setTextureRect(animation_source_);
    }

    bool AnimationEvent::update(float time_elapsed)
    {
        if (light_ != nullptr)
        {
            light_->setPosition(this->getPosition());
            light_->setColor(255, 255, 255, 255 * (duration_s_ - time_elapsed_) / duration_s_);
        }

        time_elapsed_ += (reversed_ ? -speed_factor_ : speed_factor_) * time_elapsed;

        auto current_frame = static_cast<short int>(time_elapsed_ * frames_number_ / duration_s_);

        if (current_frame >= frames_number_ || current_frame < 0)
        {
            return true;
        }

        switch (type_)
        {
            case AnimationType::Linear:
            {
                animation_source_.left = (is_flipped_x_ ? frame_size_.x : 0) + frame_size_.x * current_frame;
                animation_source_.top = (is_flipped_y_ ? frame_size_.y : 0);
                break;
            }
            case AnimationType::Quadratic:
            {
                animation_source_.left = (is_flipped_x_ ? frame_size_.x : 0) + frame_size_.x *
                                                                               (current_frame %
                                                                                static_cast<short int>(std::sqrt(
                                                                                        frames_number_)));
                animation_source_.top = (is_flipped_y_ ? frame_size_.y : 0) + frame_size_.y *
                                                                              (current_frame /
                                                                               static_cast<short int>(std::sqrt(
                                                                                       frames_number_)));
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

    void AnimationEvent::setReversed(bool reversed)
    {
        reversed_ = reversed;

        if (reversed_)
            time_elapsed_ = duration_s_;
        else
            time_elapsed_ = 0.0f;
    }

    void AnimationEvent::setSpeedFactor(float factor)
    {
        speed_factor_ = factor;
    }

} // namespace r3e::graphics