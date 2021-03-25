//
// Created by jul3x on 28.05.2020.
//

#include <cmath>

#include <R3E/graphics/StaticShadow.h>
#include <R3E/utils/Utils.h>


namespace r3e::graphics {
    //
    // StaticShadow
    //

    StaticShadow::StaticShadow(const sf::Vector2f& position, const sf::Vector2f& size,
                               sf::Texture* texture,
                               const sf::Color& shadow_color, int z_index,
                               short frames_number, float frame_duration) :
            AbstractDrawableObject(position, size, texture, z_index, frames_number, frame_duration),
            shape_(sf::Quads, 4)
    {
        position_ = position;

        shape_[0].texCoords = {0.0f, size.y};
        shape_[1].texCoords = {size.x, size.y};
        shape_[2].texCoords = {size.x, 0.0f};
        shape_[3].texCoords = {0.0f, 0.0f};
        shape_[0].color = shadow_color;
        shape_[1].color = shadow_color;
        shape_[2].color = shadow_color;
        shape_[3].color = shadow_color;

        if (texture != nullptr)
        {
            states_.texture = texture;
        }
    }

    const sf::Vector2f& StaticShadow::getPosition() const
    {
        return position_;
    }

    void StaticShadow::setPosition(float x, float y)
    {
        this->setPosition({x, y});
    }

    void StaticShadow::setPositionX(float x)
    {
        this->setPosition(x, this->getPosition().y);
    }

    void StaticShadow::setPositionY(float y)
    {
        this->setPosition(this->getPosition().x, y);
    }

    void StaticShadow::setVisibility(const sf::View& view)
    {
        is_visible_ = utils::geo::AABB(view.getCenter(), view.getSize() + sf::Vector2f{300.0f, 300.0f},
                                       this->getPosition(), static_cast<sf::Vector2f>(frame_size_)) > 0;
    }

    void StaticShadow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape_, states_);
    }

    //
    // StaticTextureShadow
    //

    TransformedTextureShadow::TransformedTextureShadow(const sf::Vector2f& position, const sf::Vector2f& size,
                                                       float shadow_direction, float shadow_length_factor,
                                                       sf::Texture* texture_name, const sf::Color& shadow_color,
                                                       int z_index, short frames_number, float frame_duration) :
            StaticShadow(position, size, texture_name, shadow_color, z_index, frames_number, frame_duration)
    {
        shadow_direction_ = shadow_direction;
        shadow_length_factor_ = shadow_length_factor;

        shape_[0].position = position + sf::Vector2f{-size.x / 2.0f, size.y / 2.0f};
        shape_[1].position = position + sf::Vector2f{size.x / 2.0f, size.y / 2.0f};
        shape_[2].position =
                position + sf::Vector2f{size.x / 2.0f + std::sin(shadow_direction) * size.y * shadow_length_factor_,
                                        size.y / 2.0f - std::cos(shadow_direction) * size.y * shadow_length_factor_};
        shape_[3].position =
                position + sf::Vector2f{-size.x / 2.0f + std::sin(shadow_direction) * size.y * shadow_length_factor_,
                                        size.y / 2.0f - std::cos(shadow_direction) * size.y * shadow_length_factor_};


    }

    void TransformedTextureShadow::setPosition(const sf::Vector2f& position)
    {
        position_ = position;
        shape_[0].position = position + sf::Vector2f{-frame_size_.x / 2.0f, frame_size_.y / 2.0f};
        shape_[1].position = position + sf::Vector2f{frame_size_.x / 2.0f, frame_size_.y / 2.0f};
        shape_[2].position = position + sf::Vector2f{
                frame_size_.x / 2.0f + std::sin(shadow_direction_) * frame_size_.y * shadow_length_factor_,
                frame_size_.y / 2.0f - std::cos(shadow_direction_) * frame_size_.y * shadow_length_factor_};
        shape_[3].position = position + sf::Vector2f{
                -frame_size_.x / 2.0f + std::sin(shadow_direction_) * frame_size_.y * shadow_length_factor_,
                frame_size_.y / 2.0f - std::cos(shadow_direction_) * frame_size_.y * shadow_length_factor_};

    }

    void TransformedTextureShadow::changeTexture(sf::Texture* texture, bool reset)
    {
        states_.texture = texture;
    }

    bool TransformedTextureShadow::updateAnimation(float time_elapsed, float animation_speed_factor)
    {
        if (frames_number_ <= 1)
            return true;

        time_elapsed_ += time_elapsed * animation_speed_factor;

        auto frames_to_add = static_cast<short int>(time_elapsed_ / frame_duration_);
        time_elapsed_ -= frames_to_add * frame_duration_;

        current_frame_ = current_frame_ + frames_to_add;

        if (current_frame_ >= frames_number_)
        {
            current_frame_ -= frames_number_;
        }

        animation_source_.left = (is_flipped_x_ ? 1 : 0) * frame_size_.x + frame_size_.x * current_frame_;
        animation_source_.top = (is_flipped_y_ ? frame_size_.y : 0);

        shape_[0].texCoords = {static_cast<float>(animation_source_.left), static_cast<float>(frame_size_.y)};
        shape_[1].texCoords =
                {static_cast<float>(animation_source_.left) + (is_flipped_x_ ? -frame_size_.x : frame_size_.x),
                 static_cast<float>(frame_size_.y)};
        shape_[2].texCoords =
                {static_cast<float>(animation_source_.left) + (is_flipped_x_ ? -frame_size_.x : frame_size_.x), 0.0f};
        shape_[3].texCoords = {static_cast<float>(animation_source_.left), 0.0f};

        return true;
    }

    //
    // StaticTextureShadow
    //

    StaticTextureShadow::StaticTextureShadow(const sf::Vector2f& position, const sf::Vector2f& size,
                                             sf::Texture* texture_name, const sf::Color& shadow_color,
                                             int z_index, short frames_number, float frame_duration) :
            StaticShadow(position, size, texture_name, shadow_color, z_index, frames_number, frame_duration)
    {
        shape_[0].position = position + sf::Vector2f{-size.x / 2.0f, size.y / 2.0f};
        shape_[1].position = position + sf::Vector2f{size.x / 2.0f, size.y / 2.0f};
        shape_[2].position = position + sf::Vector2f{size.x / 2.0f, -size.y / 2.0f};
        shape_[3].position = position + sf::Vector2f{-size.x / 2.0f, -size.y / 2.0f};
    }

    void StaticTextureShadow::setPosition(const sf::Vector2f& position)
    {
        position_ = position;
        shape_[0].position = position + sf::Vector2f{-frame_size_.x / 2.0f, frame_size_.y / 2.0f};
        shape_[1].position = position + sf::Vector2f{frame_size_.x / 2.0f, frame_size_.y / 2.0f};
        shape_[2].position = position + sf::Vector2f{frame_size_.x / 2.0f, frame_size_.y / 2.0f};
        shape_[3].position = position + sf::Vector2f{-frame_size_.x / 2.0f, -frame_size_.y / 2.0f};
    }

    void StaticTextureShadow::changeTexture(sf::Texture* texture, bool reset)
    {
        states_.texture = texture;
    }

    bool StaticTextureShadow::updateAnimation(float time_elapsed, float animation_speed_factor)
    {
        if (frames_number_ <= 1)
            return true;

        time_elapsed_ += time_elapsed * animation_speed_factor;

        auto frames_to_add = static_cast<short int>(time_elapsed_ / frame_duration_);
        time_elapsed_ -= frames_to_add * frame_duration_;

        current_frame_ = current_frame_ + frames_to_add;

        if (current_frame_ >= frames_number_)
        {
            current_frame_ -= frames_number_;
        }

        animation_source_.left = (is_flipped_x_ ? 1 : 0) * frame_size_.x + frame_size_.x * current_frame_;
        animation_source_.top = (is_flipped_y_ ? frame_size_.y : 0);

        shape_[0].texCoords = {static_cast<float>(animation_source_.left), static_cast<float>(frame_size_.y)};
        shape_[1].texCoords =
                {static_cast<float>(animation_source_.left) + (is_flipped_x_ ? -frame_size_.x : frame_size_.x),
                 static_cast<float>(frame_size_.y)};
        shape_[2].texCoords =
                {static_cast<float>(animation_source_.left) + (is_flipped_x_ ? -frame_size_.x : frame_size_.x), 0.0f};
        shape_[3].texCoords = {static_cast<float>(animation_source_.left), 0.0f};

        return true;
    }
} // namespace r3e::graphics