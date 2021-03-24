//
// Created by jul3x on 02.10.20.
//

#include <common/ResourceManager.h>

#include <common/misc/BonusText.h>


BonusText::BonusText(const sf::Vector2f& pos, const std::string& text) :
        text_(text, RM.getFont(), CONF<float>("graphics/bonus_text_size")),
        time_elapsed_(CONF<float>("graphics/bonus_text_duration"))
{
    text_.setFillColor(sf::Color::White);
    text_.setPosition(pos + CONF<sf::Vector2f>("graphics/bonus_text_offset"));

    text_.setOrigin(text_.getLocalBounds().width / 2.0f, 0.0f);
}

bool BonusText::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;

    text_.setPosition(text_.getPosition() + CONF<sf::Vector2f>("graphics/bonus_text_velocity") * time_elapsed);
    text_.setFillColor(sf::Color(255, 255, 255, time_elapsed_ / CONF<float>("graphics/bonus_text_duration") * 255));

    return time_elapsed_ > 0.0f;
}

void BonusText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(text_, states);
}