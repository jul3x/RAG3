//
// Created by jul3x on 10.04.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <common/ui/Achievement.h>


Achievement::Achievement(const sf::Vector2f& position,
                         const std::string& title,
                         const std::string& text,
                         sf::Texture* tex) :
        AbstractDrawableObject(sf::Vector2f{position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y} +
                               CONF<sf::Vector2f>("graphics/achievement_img_pos"),
                               CONF<sf::Vector2f>("graphics/achievement_img_size"),
                               tex),
        position_(CONF<float>("graphics/achievement_show_speed")),
        title_text_(title, RM.getFont(), static_cast<int>(CONF<float>("graphics/achievement_title_text_size"))),
        text_text_(text, RM.getFont(), static_cast<int>(CONF<float>("graphics/achievement_text_text_size"))),
        bg_(sf::Quads, 4),
        time_elapsed_(CONF<float>("graphics/achievement_show_time"))
{
    bg_[0].position = sf::Vector2f(position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y);
    bg_[1].position = sf::Vector2f(position.x - CONF<sf::Vector2f>("graphics/achievement_size").x,
                                   -CONF<sf::Vector2f>("graphics/achievement_size").y);
    bg_[2].position = sf::Vector2f(position.x - CONF<sf::Vector2f>("graphics/achievement_size").x,
                                   -CONF<sf::Vector2f>("graphics/achievement_size").y);
    bg_[3].position = sf::Vector2f(position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y);

    bg_[0].color = sf::Color(CONF<int>("graphics/achievement_top_color"));
    bg_[1].color = sf::Color(CONF<int>("graphics/achievement_top_color"));
    bg_[2].color = sf::Color(CONF<int>("graphics/achievement_bottom_color"));
    bg_[3].color = sf::Color(CONF<int>("graphics/achievement_bottom_color"));

    position_.setForcedState({position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y});
    position_.setState({position.x, position.y});

    title_text_.setFillColor(sf::Color(CONF<int>("graphics/achievement_title_color")));
    title_bounds_ = title_text_.getLocalBounds();
    title_text_.setPosition(sf::Vector2f(position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y)
                            + CONF<sf::Vector2f>("graphics/achievement_title_pos"));

    text_text_.setFillColor(sf::Color(CONF<int>("graphics/achievement_text_color")));
    text_text_.setPosition(sf::Vector2f(position.x, -CONF<sf::Vector2f>("graphics/achievement_size").y)
                           + CONF<sf::Vector2f>("graphics/achievement_text_pos"));

    this->changeOrigin({CONF<sf::Vector2f>("graphics/achievement_size").x, 0.0f});
}

bool Achievement::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
    position_.update(time_elapsed);
    auto& curr_pos = position_.getState();

    bg_[0].position.y = 0.0f + curr_pos.y;
    bg_[1].position.y = 0.0f + curr_pos.y;
    bg_[2].position.y = -CONF<sf::Vector2f>("graphics/achievement_size").y + curr_pos.y;
    bg_[3].position.y = -CONF<sf::Vector2f>("graphics/achievement_size").y + curr_pos.y;

    title_text_.setPosition(curr_pos + CONF<sf::Vector2f>("graphics/achievement_title_pos") -
                            sf::Vector2f{title_bounds_.width / 2.0f, 0});

    text_text_.setPosition(curr_pos + CONF<sf::Vector2f>("graphics/achievement_text_pos"));
    shape_.setPosition(curr_pos + CONF<sf::Vector2f>("graphics/achievement_img_pos"));

    if (time_elapsed_ < 0.0f)
    {
        position_.setState({position_.getState().x, -10.0f});

        return curr_pos.y > 0.0f;
    }

    return true;
}

void Achievement::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(bg_, states);
    target.draw(shape_, states);
    target.draw(title_text_, states);
    target.draw(text_text_, states);
}
