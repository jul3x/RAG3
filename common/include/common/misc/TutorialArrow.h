//
// Created by jul3x on 12.04.21.
//

#ifndef RAG3_GAME_INCLUDE_MISC_TUTORIALARROW_H
#define RAG3_GAME_INCLUDE_MISC_TUTORIALARROW_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

class TutorialArrow : public sf::Drawable {
public:
    explicit TutorialArrow(AbstractPhysicalObject* father);

    bool update(float time_elapsed);

    AbstractPhysicalObject* getFather() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    AbstractPhysicalObject* father_;
    float time_elapsed_;

    sf::Vector2f base_pos_;
    sf::Text arrow_;


};

#endif //RAG3_GAME_INCLUDE_MISC_TUTORIALARROW_H
