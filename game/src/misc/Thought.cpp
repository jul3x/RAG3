//
// Created by jul3x on 01.04.20.
//

#include <common/ResourceManager.h>

#include <misc/Thought.h>


Thought::Thought(AbstractPhysicalObject* father, const std::string& text, float duration) :
        father_(father),
        text_(text, RM.getFont(), 24),
        time_elapsed_(duration),
        AbstractPhysicalObject(father->getPosition(), {1.0f, 1.0f}, Collision::None(), nullptr)
{
}

bool Thought::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
    this->setPosition(father_->getPosition());
    text_.setPosition(father_->getPosition());
    return time_elapsed_ > 0.0f;
}

void Thought::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(text_, states);
}
