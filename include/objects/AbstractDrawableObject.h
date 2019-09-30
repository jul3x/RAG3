//
// Created by jprolejko on 16.06.19.
//


#ifndef RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
#define RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <system/ResourceManager.h>
#include <utils/Numeric.h>


class AbstractDrawableObject : public sf::Drawable {
public:
    AbstractDrawableObject() = default;

    AbstractDrawableObject(const sf::Vector2f &position,
                           const sf::Vector2f &size,
                           const std::string &texture_name) : 
        texture_(&ResourceManager::getInstance().getTexture(texture_name)) {
        shape_.setPosition(position);
        shape_.setSize(size);
        shape_.setOrigin(size.x / 2.0f, size.y / 2.0f);
        shape_.setTexture(texture_);
    }

    const sf::Vector2f& getPosition() const {
        return shape_.getPosition();
    }

    const sf::Vector2f& getSize() const {
        return shape_.getSize();
    }

    const float getRotation() const {
        return shape_.getRotation();
    }

    void setPosition(const sf::Vector2f &position) {
        shape_.setPosition(position);
    }

    void setPosition(const float x, const float y) {
        shape_.setPosition({x, y});
    }

    void setRotation(const float angle_deg) {
        shape_.setRotation(angle_deg);
    }

protected:
    sf::RectangleShape shape_;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(shape_, states);
    }

    sf::Texture *texture_;

};

#endif // RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
