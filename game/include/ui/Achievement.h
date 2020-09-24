//
// Created by jul3x on 10.04.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_ACHIEVEMENT_H
#define RAG3_GAME_INCLUDE_UI_ACHIEVEMENT_H

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <R3E/objects/StateWithInertia.h>
#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

class Achievement : public AbstractDrawableObject {
public:
    explicit Achievement(const sf::Vector2f& position,
                         const std::string& title,
                         const std::string& text,
                         sf::Texture* tex);

    bool update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::FloatRect title_bounds_;

    sf::Text title_text_, text_text_;
    sf::VertexArray bg_;

    StateWithInertia<sf::Vector2f> position_;

    float time_elapsed_;
};

#endif //RAG3_GAME_INCLUDE_UI_ACHIEVEMENT_H
