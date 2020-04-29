//
// Created by jul3x on 10.04.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <ui/Achievement.h>


Achievement::Achievement(const sf::Vector2f& position,
                         const std::string& title,
                         const std::string& text,
                         sf::Texture* tex) :
        AbstractDrawableObject({position.x + IMG_X_, -SIZE_Y_ + IMG_Y_},
                               {IMG_SIZE_X_ * CFG.get<float>("graphics/user_interface_zoom"),
                                IMG_SIZE_Y_ * CFG.get<float>("graphics/user_interface_zoom")},
                               tex),
        position_(CFG.get<float>("graphics/achievement_show_speed")),
        title_text_(title, RM.getFont(), 20),
        text_text_(text, RM.getFont(), 16),
        bg_(sf::Quads, 4),
        time_elapsed_(CFG.get<float>("graphics/achievement_show_time"))
{
    bg_[0].position = sf::Vector2f(position.x, -SIZE_Y_);
    bg_[1].position = sf::Vector2f(position.x - SIZE_X_, -SIZE_Y_);
    bg_[2].position = sf::Vector2f(position.x - SIZE_X_, -SIZE_Y_);
    bg_[3].position = sf::Vector2f(position.x, -SIZE_Y_);

    bg_[0].color = sf::Color(60, 60, 60, 255);
    bg_[1].color = sf::Color(60, 60, 60, 255);
    bg_[2].color = sf::Color(36, 36, 36, 255);
    bg_[3].color = sf::Color(36, 36, 36, 255);

    position_.setForcedState({position.x, -SIZE_Y_});
    position_.setState(position);

    title_text_.setFillColor(sf::Color(TITLE_COLOR_));
    title_bounds_ = title_text_.getLocalBounds();
    title_text_.setPosition(position.x + TITLE_X_, -SIZE_Y_ + TITLE_Y_);

    text_text_.setFillColor(sf::Color(TEXT_COLOR_));
    text_text_.setPosition(position.x + TEXT_X_, -SIZE_Y_ + TEXT_Y_);

    this->changeOrigin({SIZE_X_, 0.0f});
}

bool Achievement::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
    position_.update(time_elapsed);
    auto& curr_pos = position_.getState();

    bg_[0].position.y = 0.0f + curr_pos.y;
    bg_[1].position.y = 0.0f + curr_pos.y;
    bg_[2].position.y = -SIZE_Y_ + curr_pos.y;
    bg_[3].position.y = -SIZE_Y_ + curr_pos.y;

    title_text_.setPosition(curr_pos + sf::Vector2f{TITLE_X_ - title_bounds_.width / 2.0f, TITLE_Y_});
    text_text_.setPosition(curr_pos + sf::Vector2f{TEXT_X_, TEXT_Y_});
    shape_.setPosition(curr_pos + sf::Vector2f{IMG_X_, IMG_Y_});

    if (time_elapsed_ < 0.0f)
    {
        position_.setState({position_.getState().x, - 10.0f});

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
