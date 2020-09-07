//
// Created by jul3x on 19.05.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H

#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>


namespace r3e::graphics {

    class StaticShadow : public AbstractDrawableObject {
    public:
        StaticShadow(const sf::Vector2f& position,
                     const sf::Vector2f& size,
                     sf::Texture* texture,
                     const sf::Color& shadow_color,
                     int z_index = 0,
                     short int frames_number = 1,
                     float frame_duration = 0.0f);

        [[nodiscard]] const sf::Vector2f& getPosition() const override;

        void setPosition(const sf::Vector2f& position) override = 0;
        void setPosition(float x, float y) override;
        void setPositionX(float x) override;
        void setPositionY(float y) override;
        void setVisibility(const sf::View& view) override;

    protected:
        sf::Vector2f position_;
        sf::VertexArray shape_;
        sf::RenderStates states_;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    };

    class TransformedTextureShadow : public StaticShadow {
    public:
        TransformedTextureShadow(const sf::Vector2f& position,
                                 const sf::Vector2f& size,
                                 float shadow_direction,
                                 float shadow_length_factor,
                                 sf::Texture* texture_name,
                                 const sf::Color& shadow_color,
                                 int z_index = 0,
                                 short int frames_number = 1,
                                 float frame_duration = 0.0f);

        void setPosition(const sf::Vector2f& position) override;
        void changeTexture(sf::Texture* texture, bool reset = false) override;
        bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f) override;

    private:
        float shadow_direction_;
        float shadow_length_factor_;

    };

    class StaticTextureShadow : public StaticShadow {
    public:
        StaticTextureShadow(const sf::Vector2f& position, const sf::Vector2f& size,
                            sf::Texture* texture_name, const sf::Color& shadow_color,
                            int z_index = 0, short frames_number = 1, float frame_duration = 1.0f);

        void setPosition(const sf::Vector2f& position) override;
        void changeTexture(sf::Texture* texture, bool reset = false) override;
        bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f) override;

    };
} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_STATICSHADOW_H
