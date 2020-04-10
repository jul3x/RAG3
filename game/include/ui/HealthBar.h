//
// Created by jul3x on 21.11.19.
//

#ifndef RAG3_GAME_UI_HEALTHBAR_H
#define RAG3_GAME_UI_HEALTHBAR_H


#include <SFML/Graphics/ConvexShape.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/StateWithInertia.h>


using namespace r3e;

class HealthBar : public AbstractDrawableObject {
public:
    explicit HealthBar(const sf::Vector2f& position);

    void setMaxHealth(int max_health);

    void update(int health, float time_elapsed);

    void setPosition(float x, float y) override;

    void setPosition(const sf::Vector2f& pos) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float HEALTH_SIZE_X_ = 250.0f;
    static constexpr float HEALTH_SIZE_Y_ = 250.0f;

    int max_health_;
    StateWithInertia<float> curr_health_;

    sf::ConvexShape curr_health_shape_;
};

#endif // RAG3_GAME_UI_HEALTHBAR_H
