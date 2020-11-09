//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_FULLHUD_H
#define RAG3_GAME_INCLUDE_UI_FULLHUD_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LineEffect.h>


using namespace r3e;

class BackpackHud : public sf::Drawable {
public:
    explicit BackpackHud(const sf::Vector2f& pos, int x, int y);

    void update(float time_elapsed);
    void setOpacity(sf::Uint8 a);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<AbstractDrawableObject> placeholders_;
    std::vector<sf::Text> numbers_;
};

class SkillsHud : public sf::Drawable {
public:
    explicit SkillsHud(const sf::Vector2f& pos);

    void update(float time_elapsed);
    void show(bool no = false);
    void setColor(const sf::Color& color);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::list<graphics::LineEffect> lines_;
    std::list<sf::Text> texts_;
};

class FullHud : public sf::Drawable {
public:
    explicit FullHud(const sf::Vector2f& size);

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
