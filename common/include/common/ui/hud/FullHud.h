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
#include <common/ui/hud/SkillsHud.h>
#include <common/ui/hud/BackpackHud.h>


using namespace r3e;

class UserInterface;

class Framework;

class FullHud : public sf::Drawable {
public:
    explicit FullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size);

    virtual void update(float time_elapsed);
    virtual void show(bool show);
    [[nodiscard]] bool isShow() const;

protected:
    std::vector<std::unique_ptr<TextButton>> buttons_;
    bool show_;
    float time_elapsed_;
    sf::RectangleShape bg_;
    sf::Color bg_color_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};


class ExtendedFullHud : public FullHud {
public:
    explicit ExtendedFullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size);

    void update(float time_elapsed) override;
    void show(bool show) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    Framework* framework_;
    AbstractDrawableObject player_;
    BackpackHud backpack_hud_;
    SkillsHud skills_hud_;

};

#endif //RAG3_GAME_INCLUDE_UI_FULLHUD_H
