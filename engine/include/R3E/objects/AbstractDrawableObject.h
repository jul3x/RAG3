//
// Created by jul3x on 16.06.19.
//


#ifndef RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
#define RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


namespace r3e {
    class AbstractDrawableObject : public sf::Drawable {
    public:
        AbstractDrawableObject();

        AbstractDrawableObject(const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               sf::Texture* texture_name);

        virtual const sf::Vector2f& getPosition() const;

        virtual const sf::Vector2f& getSize() const;

        virtual float getRotation() const;

        virtual bool isVisible() const;

        virtual void setPosition(const sf::Vector2f& position);

        virtual void setPosition(float x, float y);

        virtual void setPositionX(float x);

        virtual void setPositionY(float y);

        virtual void setRotation(float angle_deg);

        virtual void setSize(const sf::Vector2f& size);

        virtual void setVisibility(const sf::View& view);

        virtual void setColor(int r, int g, int b, int a);

        virtual void changeTexture(sf::Texture* texture, bool reset = false);

        virtual void changeOrigin(const sf::Vector2f &origin);

    protected:
        sf::RectangleShape shape_;
        bool is_visible_;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    };

    struct AbstractDrawableObjectsCmp {
        bool operator()(const AbstractDrawableObject* lhs, const AbstractDrawableObject* rhs) const
        {
            return (lhs->getPosition().y < rhs->getPosition().y) ||
                   (lhs->getPosition().y == rhs->getPosition().y && lhs->getPosition().x <= rhs->getPosition().x);
        }
    };

} // namespace r3e

#endif // RAG3_ENGINE_OBJECTS_ABSTRACTDRAWABLEOBJECT_H
