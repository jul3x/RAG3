//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_FULLHUD_H
#define RAG3_GAME_INCLUDE_UI_FULLHUD_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class FullHud : public sf::Drawable {
public:
    explicit FullHud(const sf::Vector2f& size);

    void update(float time_elapsed);
    void show(bool show);
    [[nodiscard]] bool isShow() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::RectangleShape bg_;
    sf::Color bg_color_;
    bool show_;
    float time_elapsed_;

};

#endif //RAG3_GAME_INCLUDE_UI_FULLHUD_H
