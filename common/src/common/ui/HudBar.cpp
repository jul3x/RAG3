//
// Created by jul3x on 26.03.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/ui/HudBar.h>


HudBar::HudBar(const sf::Vector2f& position, const std::string& name) :
        curr_bar_shape_({SIZE_X_, SIZE_Y_}),
        curr_amount_(CONF<float>("graphics/inertial_states_change_speed") * 5.0f),
        max_amount_(0.0f),
        is_reversed_(false),
        pos_(position)
{
    curr_bar_shape_.setPosition(position);
    curr_bar_shape_.setScale(CONF<float>("graphics/user_interface_zoom"),
                             CONF<float>("graphics/user_interface_zoom"));
    curr_bar_shape_.setTexture(&RM.getTexture(name));
}

void HudBar::setMaxAmount(float max_amount)
{
    this->max_amount_ = max_amount;
}

void HudBar::update(float amount, float time_elapsed)
{
    curr_amount_.update(time_elapsed);
    curr_amount_.setState(amount);

    float percentage = std::max(curr_amount_.getState() / max_amount_, 0.0f);
    curr_bar_shape_.setSize({percentage * SIZE_X_, SIZE_Y_});

    if (!is_reversed_)
    {
        curr_bar_shape_.setTextureRect({sf::Vector2i{0, 0},
                                        sf::Vector2i{static_cast<int>(curr_bar_shape_.getLocalBounds().width),
                                                     static_cast<int>(curr_bar_shape_.getLocalBounds().height)}});
    }
    else
    {
        curr_bar_shape_.setTextureRect({sf::Vector2i{static_cast<int>(SIZE_X_ - curr_bar_shape_.getLocalBounds().width), 0}, 
                                        sf::Vector2i{static_cast<int>(SIZE_X_), static_cast<int>(SIZE_Y_)}});
        curr_bar_shape_.setPosition(pos_ + sf::Vector2f{
            (1.0f - percentage) * CONF<float>("graphics/user_interface_zoom") * SIZE_X_, 0.0f});
    }

}

void HudBar::setFreeze(bool freeze)
{
    if (freeze)
    {
        curr_bar_shape_.setFillColor(sf::Color(100, 100, 100, 100));
    }
    else
    {
        curr_bar_shape_.setFillColor(sf::Color(255, 255, 255, 255));
    }
}

void HudBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(curr_bar_shape_, states);
}

void HudBar::setPosition(float x, float y)
{
    curr_bar_shape_.setPosition(x, y);
}

void HudBar::setPosition(const sf::Vector2f& pos)
{
    curr_bar_shape_.setPosition(pos);
}

void HudBar::setReversed(bool reversed)
{
    is_reversed_ = reversed;
}
