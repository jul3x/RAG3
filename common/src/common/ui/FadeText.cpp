//
// Created by jul3x on 12.02.2022.
//

#include <common/ResourceManager.h>
#include <common/ui/FadeText.h>


FadeText::FadeText(const std::string& text, const sf::Vector2f& position, float size, float time) :
    text_(text, RM.getFont(), size),
    is_show_(false),
    opacity_(time == 0.0f ? CONF<float>("graphics/fade_text_speed") : time)
{
    opacity_.setState(0.0f);
    opacity_.setForcedState(0.0f);

    text_.setPosition(position);

    text_.setFillColor(sf::Color::Transparent);
    setPosition(position);
}

void FadeText::show(bool show)
{
    if (show == is_show_)
        return;

    opacity_.setState(show ? 1.0f : 0.0f);
    is_show_ = show;
}

void FadeText::update(float time_elapsed)
{
    opacity_.update(time_elapsed);
    auto color = sf::Color::White;
    color.a = opacity_.getState() * 255;
    text_.setFillColor(color);
}

void FadeText::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    if (opacity_.getState() > 0.01f)
        window.draw(text_, states);
}

void FadeText::setString(const std::string& str)
{
    text_.setString(str);
}

void FadeText::setPosition(const sf::Vector2f& pos)
{
    auto upgrade_bounds = text_.getGlobalBounds();
    text_.setPosition({pos.x - upgrade_bounds.width / 2.0f, pos.y});
}

