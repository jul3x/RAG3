//
// Created by jul3x on 30.09.19.
//


#include <R3E/system/AbstractResourceManager.h>
#include <R3E/utils/Geometry.h>
#include <R3E/objects/AbstractDrawableObject.h>


namespace r3e {

    AbstractDrawableObject::AbstractDrawableObject() : is_visible_(true), z_index_(0)
    {
    }


    AbstractDrawableObject::AbstractDrawableObject(const sf::Vector2f& position,
                                                   const sf::Vector2f& size,
                                                   sf::Texture* texture,
                                                   int z_index,
                                                   short int frames_number,
                                                   float frame_duration) :
            frames_number_(frames_number),
            frame_duration_(frame_duration),
            z_index_(z_index),
            frame_size_(sf::Vector2i(size.x, size.y)),
            current_frame_(0),
            time_elapsed_(0.0f),
            animation_source_({0, 0}, frame_size_),
            is_visible_(true)
    {
        shape_.setPosition(position);
        shape_.setSize(size);
        shape_.setOrigin(size.x / 2.0f, size.y / 2.0f);
        if (texture != nullptr)
        {
            shape_.setTexture(texture);

            if (frames_number > 1)
                shape_.setTextureRect(animation_source_);
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

    void AbstractDrawableObject::changeTextureRect(const sf::IntRect& rect)
    {
        shape_.setTextureRect(rect);
    }

    void AbstractDrawableObject::changeOrigin(const sf::Vector2f& origin)
    {
        shape_.setOrigin(origin);
    }

    bool AbstractDrawableObject::updateAnimation(float time_elapsed, float animation_speed_factor)
    {
        if (frames_number_ <= 1) return true;

        time_elapsed_ += time_elapsed * animation_speed_factor;

        auto frames_to_add = static_cast<short int>(time_elapsed_ / frame_duration_);
        time_elapsed_ -= frames_to_add * frame_duration_;

        current_frame_ = current_frame_ + frames_to_add;

        if (current_frame_ >= frames_number_)
        {
            current_frame_ -= frames_number_;
        }

        animation_source_.left = frame_size_.x * current_frame_;
        animation_source_.top = 0;

        shape_.setTextureRect(animation_source_);

        return true;
    }

    int AbstractDrawableObject::getZIndex() const
    {
        return z_index_;
    }

    short int AbstractDrawableObject::getFramesNumber() const
    {
        return frames_number_;
    }

    short int AbstractDrawableObject::getCurrentFrame() const
    {
        return current_frame_;
    }

    void AbstractDrawableObject::setCurrentFrame(short int frame)
    {
        if (frame >= frames_number_)
        {
            throw std::invalid_argument("[AbstractDrawableObject] Frame to set should be less then frames_number_");
        }

        current_frame_ = frame;
    }

} // namespace r3e