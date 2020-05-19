//
// Created by jul3x on 19.05.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTNING_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTNING_H


#include <SFML/Graphics/RenderTexture.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LightPoint.h>



namespace r3e::graphics {
    class Lightning : public AbstractDrawableObject {
    public:
        Lightning(const sf::Vector2f& size, const sf::Color& clear_color) :
                clear_color_(clear_color),
                AbstractDrawableObject(size / 2.0f, size, nullptr)
        {
            render_texture_.create(size.x, size.y);
            this->changeTexture((sf::Texture*) &render_texture_.getTexture(), true);
        }

        void clear()
        {
            render_texture_.clear(clear_color_);
        }

        void add(const LightPoint& light)
        {
            render_texture_.draw(light, sf::BlendAdd);
        }

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(shape_, sf::BlendMultiply);
        }

        sf::RenderTexture render_texture_;
        sf::Color clear_color_;
    };
} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTNING_H
