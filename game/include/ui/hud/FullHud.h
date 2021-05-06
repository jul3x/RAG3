//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_FULLHUD_H
#define RAG3_GAME_INCLUDE_UI_FULLHUD_H

#include <SFML/Graphics/Text.hpp>
#include <TGUI/TGUI.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LineEffect.h>

#include <common/characters/Player.h>
#include <common/ui/Tooltip.h>
#include <common/ui/TextButton.h>


using namespace r3e;

class UserInterface;
class Framework;

class BackpackHud : public sf::Drawable {
public:
    explicit BackpackHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos, int x, int y);

    void update(float time_elapsed);
    void show(bool hide = false);
    void setOpacity(sf::Uint8 a);

    void clickPlaceholder(Tooltip& tooltip);
    const std::vector<size_t>& getActiveTooltips();
    void resetActiveTooltips();
    void combineBackpackItems(size_t first, size_t second);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<AbstractDrawableObject> placeholders_;
    std::list<std::pair<AbstractDrawableObject, std::string>> weapons_;
    std::vector<Tooltip> tooltips_;
    std::vector<sf::Text> numbers_;

    UserInterface* ui_;
    Framework* framework_;
};

class SkillsHud : public sf::Drawable {
public:
    explicit SkillsHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos);

    void update(float time_elapsed);
    void show(bool hide = false);
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::string texts_placeholders_[4] = {"Intelligence", "Heart", "Strength", "Agility"};
    std::string texts_tooltips_[4] = {"More intelligence comes with better understanding.\n"
                                      "Increases your ability of using RAG3 crystals to manipulate time.",
                                      "Stronger heart can improve your endurance.\n"
                                      "Increases your maximum health.",
                                      "Stronger muscles are very desired in abandoned world...\n"
                                      "Higher number of this skill improves your melee damage\n"
                                      "and decreases your body effects after shots and hits.",
                                      "Faster you run, more likely you survive.\n"
                                      "Improves your running skills."};
    Player::Skills skills_[4] =
            {Player::Skills::Intelligence, Player::Skills::Heart, Player::Skills::Strength, Player::Skills::Agility};

    std::list<graphics::LineEffect> lines_;
    std::list<sf::Text> texts_;
    std::list<Tooltip> tooltips_;
    sf::Text points_text_;
    std::list<tgui::Button::Ptr> buttons_;

    Framework* framework_;

};

class FullHud : public sf::Drawable {
public:
    explicit FullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size);

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

    std::vector<std::unique_ptr<TextButton>> buttons_;

};

#endif //RAG3_GAME_INCLUDE_UI_FULLHUD_H
