//
// Created by jprolejko on 16.06.19.
//


#ifndef RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
#define RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


class AbstractDrawableObject : public sf::Drawable {
public:
    AbstractDrawableObject() = default;

    AbstractDrawableObject(const sf::Vector2f &position,
                           const sf::Vector2f &size,
                           const std::string &texture_name);

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getSize() const;
    const float getRotation() const;
    bool isVisible() const;

    void setPosition(const sf::Vector2f &position);
    void setPosition(const float x, const float y);
    void setRotation(const float angle_deg);
    virtual void setVisibility(const sf::View &view);

protected:
    sf::RectangleShape shape_;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Texture *texture_;
    bool is_visible_;
};

#endif // RAG3_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
