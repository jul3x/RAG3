//
// Created by jul3x on 26.03.20.
//

#ifndef RAG3_GAME_UI_TIMEBAR_H
#define RAG3_GAME_UI_TIMEBAR_H


#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class TimeBar : public AbstractDrawableObject {
public:
    explicit TimeBar(const sf::Vector2f& position);

    void setMaxTime(int max_time);

    void updateTime(int time);

    void setPosition(float x, float y) override;

    void setPosition(const sf::Vector2f& pos) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float TIME_SIZE_X_ = 250.0f;
    static constexpr float TIME_SIZE_Y_ = 250.0f;

    int max_time_;

    sf::ConvexShape curr_time_;
};

#endif // RAG3_GAME_UI_TIMEBAR_H
