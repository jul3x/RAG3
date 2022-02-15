//
// Created by jul3x on 12.02.2022.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_UI_FADETEXT_H
#define RAG3_COMMON_INCLUDE_COMMON_UI_FADETEXT_H

#include <SFML/Graphics.hpp>

#include <R3E/objects/StateWithInertia.h>

using namespace r3e;

class FadeText : public sf::Drawable {
public:
    explicit FadeText(const std::string& text, const sf::Vector2f& position, float size, float time = 0.0f);

    void show(bool show = true);
    void setString(const std::string& str);
    void setPosition(const sf::Vector2f& pos);
    void update(float time_elapsed);

private:
    void draw(sf::RenderTarget& window, sf::RenderStates states) const override;

    sf::Text text_;
    StateWithInertia<float> opacity_;
    bool is_show_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_UI_FADETEXT_H
