//
// Created by jul3x on 30.09.19.
//


#include <engine/system/AbstractResourceManager.h>
#include <engine/utils/Geometry.h>
#include <engine/objects/AbstractDrawableObject.h>


namespace r3e {

    AbstractDrawableObject::AbstractDrawableObject() : is_visible_(true) {
    }


    AbstractDrawableObject::AbstractDrawableObject(const sf::Vector2f &position,
                                                   const sf::Vector2f &size,
                                                   sf::Texture *texture) :
            is_visible_(true) {
        if (texture != nullptr) {
            shape_.setPosition(position);
            shape_.setSize(size);
            shape_.setOrigin(size.x / 2.0f, size.y / 2.0f);
            shape_.setTexture(texture);
        }
    }

    const sf::Vector2f &AbstractDrawableObject::getPosition() const {
        return shape_.getPosition();
    }

    const sf::Vector2f &AbstractDrawableObject::getSize() const {
        return shape_.getSize();
    }

    float AbstractDrawableObject::getRotation() const {
        return shape_.getRotation();
    }

    bool AbstractDrawableObject::isVisible() const {
        return is_visible_;
    }

    void AbstractDrawableObject::setPosition(const sf::Vector2f &position) {
        shape_.setPosition(position);
    }

    void AbstractDrawableObject::setPosition(float x, float y) {
        shape_.setPosition({x, y});
    }

    void AbstractDrawableObject::setPositionX(float x) {
        shape_.setPosition({x, this->getPosition().y});
    }

    void AbstractDrawableObject::setPositionY(float y) {
        shape_.setPosition({this->getPosition().x, y});
    }

    void AbstractDrawableObject::setRotation(float angle_deg) {
        shape_.setRotation(angle_deg);
    }

    void AbstractDrawableObject::setSize(const sf::Vector2f &size) {
        shape_.setSize(size);
    }

    void AbstractDrawableObject::setVisibility(const sf::View &view) {
        // visibility is checked on bigger view (e.g. to avoid tunnelling)
        is_visible_ = utils::geo::AABB(view.getCenter(), view.getSize() + sf::Vector2f{300.0f, 300.0f},
                                       this->getPosition(), this->getSize()) > 0;
    }

    void AbstractDrawableObject::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        target.draw(shape_, states);
    }

    void AbstractDrawableObject::setColor(int r, int g, int b, int a) {
        shape_.setFillColor(sf::Color(r, g, b, a));
    }

} // namespace r3e