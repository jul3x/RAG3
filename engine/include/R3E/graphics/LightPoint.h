//
// Created by jul3x on 19.05.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H

#include <R3E/graphics/Graphics.h>
#include <R3E/objects/AbstractDrawableObject.h>


namespace r3e::graphics {

    class LightPoint : public AbstractDrawableObject {
    public:
        LightPoint(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) :
                AbstractDrawableObject(pos, size, texture),
                graphics_(nullptr)
        {

        }

        void registerGraphics(graphics::Graphics& graphics)
        {
            graphics_ = &graphics;
        }

        void setPosition(float x, float y) override
        {
            auto mapped_pos = graphics_->getWindow().mapCoordsToPixel({x, y}, graphics_->getCurrentView());
            auto mapped_pos_2 = graphics_->getWindow().mapPixelToCoords(mapped_pos, graphics_->getStaticView());

            AbstractDrawableObject::setPosition(mapped_pos_2);
        }

        void setPosition(const sf::Vector2f& pos) override
        {
            auto mapped_pos = graphics_->getWindow().mapCoordsToPixel(pos, graphics_->getCurrentView());
            auto mapped_pos_2 = graphics_->getWindow().mapPixelToCoords(mapped_pos, graphics_->getStaticView());
            AbstractDrawableObject::setPosition(mapped_pos_2.x, graphics_->getStaticView().getSize().y - mapped_pos_2.y);
        }

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(shape_, sf::BlendAdd);
        }

        graphics::Graphics* graphics_;

    };
} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H
