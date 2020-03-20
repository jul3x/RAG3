//
// Created by jul3x on 30.09.19.
//


#include <R3E/system/AbstractResourceManager.h>
#include <R3E/utils/Geometry.h>
#include <R3E/objects/AbstractDrawableObject.h>


namespace r3e {

    AbstractDrawableObject::AbstractDrawableObject() : is_visible_(true)
    {
    }


    AbstractDrawableObject::AbstractDrawableObject(const sf::Vector2f& position,
                                                   const sf::Vector2f& size,
                                                   sf::Texture* texture) :
            is_visible_(true)
    {
        if (texture != nullptr)
        {
            shape_.setPosition(position);
            shape_.setSize(size);
            shape_.setOrigin(size.x / 2.0f, size.y / 2.0f);
            shape_.setTexture(texture);

            // animation
            frame_size_ = sf::Vector2i(size.x, size.y);
          //  animation_period_ = 0.5f;
            max_frames_count_ = 4;
            time_elapsed_ = 0.0f;
            animation_source_ = sf::IntRect({0, 0}, frame_size_);
        }
    }

    const sf::Vector2f& AbstractDrawableObject::getPosition() const
    {
        return shape_.getPosition();
    }

    const sf::Vector2f& AbstractDrawableObject::getSize() const
    {
        return shape_.getSize();
    }

    float AbstractDrawableObject::getRotation() const
    {
        return shape_.getRotation();
    }

    bool AbstractDrawableObject::isVisible() const
    {
        return is_visible_;
    }

    void AbstractDrawableObject::setPosition(const sf::Vector2f& position)
    {
        shape_.setPosition(position);
    }

    void AbstractDrawableObject::setPosition(float x, float y)
    {
        shape_.setPosition({x, y});
    }

    void AbstractDrawableObject::setPositionX(float x)
    {
        shape_.setPosition({x, this->getPosition().y});
    }

    void AbstractDrawableObject::setPositionY(float y)
    {
        shape_.setPosition({this->getPosition().x, y});
    }

    void AbstractDrawableObject::setRotation(float angle_deg)
    {
        shape_.setRotation(angle_deg);
    }

    void AbstractDrawableObject::setSize(const sf::Vector2f& size)
    {
        shape_.setSize(size);
    }

    void AbstractDrawableObject::setVisibility(const sf::View& view)
    {
        // visibility is checked on bigger view (e.g. to avoid tunnelling)
        is_visible_ = utils::geo::AABB(view.getCenter(), view.getSize() + sf::Vector2f{300.0f, 300.0f},
                                       this->getPosition(), this->getSize()) > 0;
    }

    void AbstractDrawableObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape_, states);
    }

    void AbstractDrawableObject::setColor(int r, int g, int b, int a)
    {
        shape_.setFillColor(sf::Color(r, g, b, a));
    }

    void AbstractDrawableObject::changeTexture(sf::Texture* texture, bool reset)
    {
        shape_.setTexture(texture, reset);
    }

    void AbstractDrawableObject::changeOrigin(const sf::Vector2f &origin)
    {
        shape_.setOrigin(origin);
    }

    bool AbstractDrawableObject::updateAnimation(float time_elapsed, float current_animation_period)
    {
        time_elapsed_ += time_elapsed;

        auto current_frame = static_cast<short int>(time_elapsed_ * max_frames_count_ / current_animation_period);

        if (current_frame >= max_frames_count_)
        {
            current_frame = 0;
            time_elapsed_ -= current_animation_period;
        }

        animation_source_.left = frame_size_.x * current_frame;
        animation_source_.top = 0;

        shape_.setTextureRect(animation_source_);

        return true;
    }

} // namespace r3e