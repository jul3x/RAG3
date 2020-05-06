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

    static constexpr float SIZE_X_ = 350.0f;
    static constexpr float SIZE_Y_ = 100.0f;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float IMG_SIZE_X_ = 70.0f;
    static constexpr float IMG_SIZE_Y_ = 70.0f;

    static constexpr float IMG_X_ = 15.0f;
    static constexpr float IMG_Y_ = -85.0f;

    static constexpr float TITLE_X_ = -140.0f;
    static constexpr float TITLE_Y_ = -90.0f;

    static constexpr float TEXT_X_ = -250.0f;
    static constexpr float TEXT_Y_ = -50.0f;

    static constexpr sf::Uint32 TITLE_COLOR_ = 0xFFFFFFFF;
    static constexpr sf::Uint32 TEXT_COLOR_ = 0xCCCCCCFF;

    sf::FloatRect title_bounds_;

    sf::Text title_text_, text_text_;
    sf::VertexArray bg_;

    StateWithInertia<sf::Vector2f> position_;

    float time_elapsed_;
};

#endif //RAG3_GAME_INCLUDE_UI_ACHIEVEMENT_H
