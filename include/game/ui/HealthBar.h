//
// Created by jul3x on 21.11.19.
//

#ifndef RAG3_GAME_UI_HEALTHBAR_H
#define RAG3_GAME_UI_HEALTHBAR_H


#include <SFML/Graphics/Text.hpp>

#include <engine/objects/AbstractDrawableObject.h>


using namespace r3e;

class HealthBar : public AbstractDrawableObject {
public:
    explicit HealthBar(const sf::Vector2f &position);

    void setMaxHealth(int max_health);

    void updateHealth(int health);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    static constexpr float HEALTH_SIZE_X_ = 200.0f;
    static constexpr float HEALTH_SIZE_Y_ = 20.0f;

    sf::Text health_text_;
    int max_health_;
};

#endif // RAG3_GAME_UI_HEALTHBAR_H
