//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_STATSHUD_H
#define RAG3_GAME_INCLUDE_UI_STATSHUD_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/StateWithInertia.h>
#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class StatsHud : public AbstractDrawableObject {
public:
    explicit StatsHud(const sf::Vector2f& position);

    void update(int deaths, int crystals, float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 590.0f;
    static constexpr float SIZE_Y_ = 250.0f;

    static constexpr float DEATHS_X_ = 100.0f;
    static constexpr float DEATHS_Y_ = 20.0f;

    static constexpr float CRYSTALS_X_ = 100.0f;
    static constexpr float CRYSTALS_Y_ = 75.0f;

    StateWithInertia<float> deaths_, crystals_;
    sf::Text deaths_text_, crystals_text_;

};

#endif //RAG3_GAME_INCLUDE_UI_STATSHUD_H
