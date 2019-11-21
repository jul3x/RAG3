//
// Created by jprolejko on 21.11.19.
//

#ifndef RAG3_OBJECTS_HEALTHBAR_H
#define RAG3_OBJECTS_HEALTHBAR_H


#include <SFML/Graphics/Text.hpp>

#include <objects/AbstractDrawableObject.h>


class HealthBar : public AbstractDrawableObject {
public:
    HealthBar(const sf::Vector2f &position);

    void setMaxHealth(int max_health);
    void updateHealth(int health);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static constexpr float HEALTH_SIZE_X_ = 200.0f;
    static constexpr float HEALTH_SIZE_Y_ = 20.0f;

    sf::Text health_text_;
    int max_health_;
};

#endif // RAG3_OBJECTS_HEALTHBAR_H
