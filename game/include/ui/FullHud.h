//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_FULLHUD_H
#define RAG3_GAME_INCLUDE_UI_FULLHUD_H

#include <SFML/Graphics/Text.hpp>
#include <TGUI/TGUI.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LineEffect.h>

#include <characters/Player.h>
#include <ui/Tooltip.h>


using namespace r3e;

class BackpackHud : public sf::Drawable {
public:
    explicit BackpackHud(tgui::Gui* gui, tgui::Theme* theme, const sf::Vector2f& pos, int x, int y);

    void update(float time_elapsed);
    void show(bool hide = false);
    void setOpacity(sf::Uint8 a);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<AbstractDrawableObject> placeholders_;
    std::vector<Tooltip> tooltips_;
    std::vector<sf::Text> numbers_;
};

class SkillsHud : public sf::Drawable {
public:
    explicit SkillsHud(tgui::Gui* gui, tgui::Theme* theme, const sf::Vector2f& pos);

    void update(float time_elapsed);
    void show(bool hide = false);
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::string texts_placeholders_[4] = {"Intelligence: ", "Heart: ", "Strength: ", "Agility: "};
    Player::Skills skills_[4] = {Player::Skills::Intelligence, Player::Skills::Heart, Player::Skills::Strength, Player::Skills::Agility};

    std::list<graphics::LineEffect> lines_;
    std::list<sf::Text> texts_;
    sf::Text points_text_;
    std::list<tgui::Button::Ptr> buttons_;

};

class FullHud : public sf::Drawable {
public:
    explicit FullHud(tgui::Gui* gui, tgui::Theme* theme, const sf::Vector2f& size);

    void update(float time_elapsed);
    void show(bool show);
    [[nodiscard]] bool isShow() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    AbstractDrawableObject player_;
    BackpackHud backpack_hud_;
    SkillsHud skills_hud_;
    sf::RectangleShape bg_;
    sf::Color bg_color_;
    bool show_;
    float time_elapsed_;

};

#endif //RAG3_GAME_INCLUDE_UI_FULLHUD_H
