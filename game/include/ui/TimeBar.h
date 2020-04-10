//
// Created by jul3x on 26.03.20.
//

#ifndef RAG3_GAME_UI_TIMEBAR_H
#define RAG3_GAME_UI_TIMEBAR_H


#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/StateWithInertia.h>


using namespace r3e;

class TimeBar : public AbstractDrawableObject {
public:
    explicit TimeBar(const sf::Vector2f& position);

    void setMaxTime(float max_time);

    void update(float time, float time_elapsed);

    void setFreeze(bool freeze);

    void setPosition(float x, float y) override;

    void setPosition(const sf::Vector2f& pos) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float TIME_SIZE_X_ = 250.0f;
    static constexpr float TIME_SIZE_Y_ = 250.0f;

    float max_time_;
    StateWithInertia<float> curr_time_;

    sf::ConvexShape curr_time_shape_;
};

#endif // RAG3_GAME_UI_TIMEBAR_H
