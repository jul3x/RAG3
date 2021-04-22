//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_SMALLBACKPACKHUD_H
#define RAG3_GAME_INCLUDE_UI_SMALLBACKPACKHUD_H

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>

#include <common/characters/Player.h>
#include <common/ui/Tooltip.h>


using namespace r3e;

class SmallBackpackHud : public AbstractDrawableObject {
public:
    explicit SmallBackpackHud(Player* player, const sf::Vector2f& position);

    void registerGui(Framework* framework);
    void update(float time_elapsed);
    void doShow(bool hide);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 590.0f;
    static constexpr float SIZE_Y_ = 250.0f;
    const std::string NAMES_[3] = {"health", "more_speed", "rag3"};

    std::vector<Special> objects_;
    std::vector<sf::Text> numbers_;
    std::vector<Tooltip> tooltips_;

    Player* player_;

};

#endif //RAG3_GAME_INCLUDE_UI_STATSHUD_H
