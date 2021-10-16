//
// Created by jul3x on 26.03.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_TIMEBAR_H
#define RAG3_GAME_INCLUDE_UI_TIMEBAR_H


#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/StateWithInertia.h>


using namespace r3e;

class HudBar : public sf::Drawable {
public:
    explicit HudBar(const sf::Vector2f& position, const std::string& tex_name);

    void setReversed(bool reversed);

    void setMaxAmount(float max_amount);

    void update(float amount, float time_elapsed);

    void setFreeze(bool freeze);

    void setPosition(float x, float y);

    void setPosition(const sf::Vector2f& pos);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 228.0f;
    static constexpr float SIZE_Y_ = 19.0f;

    float max_amount_;
    StateWithInertia<float> curr_amount_;
    bool is_reversed_;

    sf::Vector2f pos_;
    sf::RectangleShape curr_bar_shape_;
};

#endif //RAG3_GAME_INCLUDE_UI_TIMEBAR_H
