//
// Created by jul3x on 16.06.19.
//


#ifndef RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
#define RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


class AbstractDrawableObject : public sf::Drawable {
public:
    AbstractDrawableObject() = default;

    AbstractDrawableObject(const sf::Vector2f& position,
                           const sf::Vector2f& size,
                           sf::Texture* texture_name);

    const sf::Vector2f& getPosition() const;

    const sf::Vector2f& getSize() const;

    float getRotation() const;

    bool isVisible() const;

    void setPosition(const sf::Vector2f& position);

    void setPosition(float x, float y);

    void setRotation(float angle_deg);

    virtual void setVisibility(const sf::View& view);

protected:
    sf::RectangleShape shape_;
    bool is_visible_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};

#endif // RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
