//
// Created by jprolejko on 30.09.19.
//


#include <engine/system/ResourceManager.h>
#include <engine/utils/Geometry.h>
#include <engine/objects/AbstractDrawableObject.h>


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

void AbstractDrawableObject::setPosition(const float x, const float y)
{
    shape_.setPosition({x, y});
}

void AbstractDrawableObject::setRotation(const float angle_deg)
{
    shape_.setRotation(angle_deg);
}

void AbstractDrawableObject::setVisibility(const sf::View& view)
{
    // visibility is checked on bigger view (e.g. to avoid tunnelling of enemies)
    is_visible_ = utils::AABB(view.getCenter(), view.getSize() + sf::Vector2f{300.0f, 300.0f},
                              this->getPosition(), this->getSize());
}

void AbstractDrawableObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (is_visible_)
    {
        target.draw(shape_, states);
    }
}

