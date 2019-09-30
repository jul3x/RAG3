//
// Created by jprolejko on 30.09.19.
//


#include <system/ResourceManager.h>

#include <objects/AbstractDrawableObject.h>


AbstractDrawableObject::AbstractDrawableObject(const sf::Vector2f &position,
                                               const sf::Vector2f &size,
                                               const std::string &texture_name) : 
        texture_(&ResourceManager::getInstance().getTexture(texture_name)) {
    shape_.setPosition(position);
    shape_.setSize(size);
    shape_.setOrigin(size.x / 2.0f, size.y / 2.0f);
    shape_.setTexture(texture_);
}

const sf::Vector2f& AbstractDrawableObject::getPosition() const {
    return shape_.getPosition();
}

const sf::Vector2f& AbstractDrawableObject::getSize() const {
    return shape_.getSize();
}

const float AbstractDrawableObject::getRotation() const {
    return shape_.getRotation();
}

void AbstractDrawableObject::setPosition(const sf::Vector2f &position) {
    shape_.setPosition(position);
}

void AbstractDrawableObject::setPosition(const float x, const float y) {
    shape_.setPosition({x, y});
}

void AbstractDrawableObject::setRotation(const float angle_deg) {
    shape_.setRotation(angle_deg);
}

void AbstractDrawableObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape_, states);
}

