//
// Created by jul3x on 26.03.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <ui/TimeBar.h>


TimeBar::TimeBar(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {TIME_SIZE_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                                TIME_SIZE_Y_ * CFG.getFloat("graphics/user_interface_zoom")},
                               &RM.getTexture("full_time_bar")),
        curr_time_(4)
{

    curr_time_.setPosition(position);
    curr_time_.setPoint(0, {0.0f, 0.0f});
    curr_time_.setPoint(3, {-TIME_SIZE_X_ / 2.0f, -TIME_SIZE_Y_ / 2.0f});
    curr_time_.setTexture(&RM.getTexture("current_time_bar"));
}

void TimeBar::setMaxTime(int max_time)
{
    this->max_time_ = max_time;
}

void TimeBar::updateTime(int time)
{
    float percentage = static_cast<float>(time) / static_cast<float>(max_time_);
    float theta = M_PI_2 + M_PI_4 - percentage * M_PI;

    if (theta < M_PI_4)
    {
        curr_time_.setPoint(1, TIME_SIZE_X_ / 2.0f / std::cos(theta) * sf::Vector2f{std::cos(theta),
                                                                                    -std::sin(theta)});
        curr_time_.setPoint(2, {TIME_SIZE_X_ / 2.0f, -TIME_SIZE_Y_ / 2.0f});
        curr_time_.setTextureRect({{0, 0}, {static_cast<int>(curr_time_.getLocalBounds().width),
                                            static_cast<int>(curr_time_.getLocalBounds().height)}});
    }
    else
    {
        curr_time_.setPoint(1, TIME_SIZE_X_ / 2.0f / std::sin(theta) * sf::Vector2f{std::cos(theta),
                                                                                    -std::sin(theta)});
        curr_time_.setPoint(2, {-TIME_SIZE_X_ / 2.0f, -TIME_SIZE_Y_ / 2.0f});

        curr_time_.setTextureRect({
            sf::Vector2i{0, 0},
            sf::Vector2i{static_cast<int>(curr_time_.getLocalBounds().width),
                         static_cast<int>(curr_time_.getLocalBounds().height)}});
    }



}

void TimeBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(curr_time_, states);
}

void TimeBar::setPosition(float x, float y)
{
    AbstractDrawableObject::setPosition(x, y);
    curr_time_.setPosition(x, y);
}

void TimeBar::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);
    curr_time_.setPosition(pos);
}