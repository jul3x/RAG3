//
// Created by jul3x on 01.04.20.
//

#include <common/ResourceManager.h>

#include <misc/Thought.h>


Thought::Thought(AbstractPhysicalObject* father, const std::string& text, float duration) :
        father_(father),
        text_(text, RM.getFont(), TEXT_SIZE_),
        time_elapsed_(duration),
        AbstractPhysicalObject(father->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_}, {SIZE_X_, SIZE_Y_BOTTOM_}, Collision::None(), &RM.getTexture("thought_bottom")),
        top_(father->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_}, {SIZE_X_, SIZE_Y_TOP_}, &RM.getTexture("thought_top"))
{
    this->changeOrigin({0.0f, SIZE_Y_BOTTOM_});
    top_.changeOrigin({0.0f, SIZE_Y_TOP_});

    text_.setFillColor(sf::Color::Black);

    auto count = std::count(text.begin(), text.end(), '\n');
    center_.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        center_.emplace_back(father->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_ - SIZE_Y_BOTTOM_ - i * SIZE_Y_CENTER_}, sf::Vector2f{SIZE_X_, SIZE_Y_CENTER_}, &RM.getTexture("thought"));
        center_.at(i).changeOrigin({0.0f, SIZE_Y_CENTER_});
    }

    top_.setPosition(father->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_ - count * SIZE_Y_CENTER_ - SIZE_Y_BOTTOM_});
}

bool Thought::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
    this->setPosition(father_->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_});

    for (size_t i = 0; i < center_.size(); ++i)
    {
        center_.at(i).setPosition(father_->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_ - SIZE_Y_BOTTOM_ - i * SIZE_Y_CENTER_});
    }

    top_.setPosition(father_->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_, GLOBAL_OFFSET_Y_ - center_.size() * SIZE_Y_CENTER_ - SIZE_Y_BOTTOM_});
    text_.setPosition(father_->getPosition() + sf::Vector2f{GLOBAL_OFFSET_X_ + TEXT_MARGIN_, GLOBAL_OFFSET_Y_ - center_.size() * SIZE_Y_CENTER_ - SIZE_Y_BOTTOM_ + SIZE_Y_TOP_ / 2.0f - TEXT_SIZE_});

    return time_elapsed_ > 0.0f;
}

void Thought::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);

    for (auto& center : center_)
        target.draw(center, states);

    target.draw(top_, states);
    target.draw(text_, states);
}
