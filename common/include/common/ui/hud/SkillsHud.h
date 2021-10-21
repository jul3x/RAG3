//
// Created by jul3x on 16.10.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_SKILLSHUD_H
#define RAG3_GAME_INCLUDE_UI_SKILLSHUD_H

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

class SkillsHud : public sf::Drawable {
public:
    explicit SkillsHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos);

    void update(float time_elapsed);
    void show(bool show = true);
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::string texts_tooltips_[4] = {"More intelligence comes with better understanding.\n"
                                      "Increases your ability of using RAG3 crystals to manipulate time.",
                                      "Stronger heart can improve your endurance.\n"
                                      "Increases your maximum health.",
                                      "Stronger muscles are very desired in abandoned world...\n"
                                      "Higher number of this skill improves your melee damage\n"
                                      "and decreases your body effects after shots and hits.",
                                      "Faster you run, more likely you survive.\n"
                                      "Improves your running skills."};

    std::list<graphics::LineEffect> lines_;
    std::list<sf::Text> texts_;
    std::list<Tooltip> tooltips_;
    sf::Text points_text_;
    std::list<tgui::Button::Ptr> buttons_;

    Framework* framework_;

};

#endif //RAG3_GAME_INCLUDE_UI_SKILLSHUD_H
