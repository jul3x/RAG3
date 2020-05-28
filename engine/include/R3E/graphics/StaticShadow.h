//
// Created by jul3x on 19.05.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H

#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>


namespace r3e::graphics {

    class StaticShadow : public sf::Drawable {
    public:
        StaticShadow(const sf::Vector2f& position,
                     const sf::Vector2f& size,
                     float shadow_direction,
                     sf::Texture* texture_name,
                     const sf::Color& shadow_color,
                     short int frames_number = 1,
                     float frame_duration = 0.0f);

        [[nodiscard]] virtual const sf::Vector2f& getPosition() const;
        [[nodiscard]] virtual const sf::Vector2i& getSize() const;
        [[nodiscard]] virtual short int getFramesNumber() const;
        [[nodiscard]] virtual short int getCurrentFrame() const;
        [[nodiscard]] virtual bool isVisible() const;

        virtual void setPosition(const sf::Vector2f& position);
        virtual void setPosition(float x, float y);
        virtual void setPositionX(float x);
        virtual void setPositionY(float y);
        virtual void setScale(float scale);
        virtual void setCurrentFrame(short int frame);
        virtual void setVisibility(const sf::View& view);

        virtual void changeTexture(sf::Texture* texture, bool reset = false);
        virtual void changeTextureRect(const sf::IntRect &rect);
        virtual void changeOrigin(const sf::Vector2f &origin);

        virtual bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f);

    protected:
        sf::Vector2f position_;
        sf::VertexArray shape_;
        float shadow_direction_;
        bool is_visible_;
        sf::RenderStates states_;
        float scale_;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        float time_elapsed_;
        short int current_frame_;
        sf::Vector2i frame_size_;
        sf::IntRect animation_source_;

        short int frames_number_;
        float frame_duration_;
    };
} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H
