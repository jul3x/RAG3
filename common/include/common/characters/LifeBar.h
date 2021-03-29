//
// Created by jul3x on 29.03.2021.
//

#ifndef RAG3_COMMON_CHARACTERS_LIFEBAR_H
#define RAG3_COMMON_CHARACTERS_LIFEBAR_H


#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <R3E/objects/StateWithInertia.h>


class LifeBar : public sf::Drawable {
public:
    LifeBar();

    explicit LifeBar(const std::string& name, float max_health);

    void update(float time_elapsed);

    void setMaxHealth(float max_health);

    void setHealth(float health);

    void setPosition(const sf::Vector2f& pos);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    r3e::StateWithInertia<float> health_{};
    float max_health_{};
    sf::VertexArray line_;
    sf::Text name_;

    sf::Vector2f pos_;

};


#endif //RAG3_COMMON_CHARACTERS_LIFEBAR_H
