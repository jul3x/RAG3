//
// Created by jul3x on 02.10.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_BONUSTEXT_H
#define RAG3_GAME_INCLUDE_MISC_BONUSTEXT_H

#include <SFML/Graphics/Text.hpp>


using namespace r3e;

class BonusText : public sf::Drawable {
public:
    BonusText(const sf::Vector2f& pos, const std::string& text, float time = 0.0f);

    bool update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    float time_elapsed_, full_time_;
    sf::Text text_;

};

#endif //RAG3_GAME_INCLUDE_MISC_BONUSTEXT_H
