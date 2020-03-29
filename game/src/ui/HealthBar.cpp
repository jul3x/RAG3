//
// Created by jul3x on 21.11.19.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <ui/HealthBar.h>


HealthBar::HealthBar(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {HEALTH_SIZE_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                                HEALTH_SIZE_Y_ * CFG.getFloat("graphics/user_interface_zoom")},
                               &RM.getTexture("full_health_bar")),
        curr_health_(4)
{

    curr_health_.setPosition(position);
    curr_health_.setPoint(0, {0.0f, 0.0f});
    curr_health_.setPoint(3, {HEALTH_SIZE_X_ / 2.0f, -HEALTH_SIZE_Y_ / 2.0f});
    curr_health_.setTexture(&RM.getTexture("current_health_bar"));
}

void HealthBar::setMaxHealth(int max_health)
{
    this->max_health_ = max_health;
}

void HealthBar::updateHealth(int health)
{
    float percentage = static_cast<float>(health) / static_cast<float>(max_health_);
    float theta = M_PI_4 + percentage * M_PI;

    if (theta > M_PI_2 + M_PI_4)
    {
        curr_health_.setPoint(1, -HEALTH_SIZE_X_ / 2.0f / std::cos(theta) * sf::Vector2f{std::cos(theta),
                                                                                         -std::sin(theta)});
        curr_health_.setPoint(2, {-HEALTH_SIZE_X_ / 2.0f, -HEALTH_SIZE_Y_ / 2.0f});
        curr_health_.setTextureRect({{0, 0}, {static_cast<int>(curr_health_.getLocalBounds().width),
                                         static_cast<int>(curr_health_.getLocalBounds().height)}});
    }
    else
    {
        curr_health_.setPoint(1, -HEALTH_SIZE_X_ / 2.0f / -std::sin(theta) * sf::Vector2f{std::cos(theta),
                                                                                          -std::sin(theta)});
        curr_health_.setPoint(2, {HEALTH_SIZE_X_ / 2.0f, -HEALTH_SIZE_Y_ / 2.0f});

        curr_health_.setTextureRect({
            sf::Vector2i{static_cast<int>(HEALTH_SIZE_X_) / 2 +
                         std::min<int>(0, static_cast<int>(curr_health_.getPoint(1).x)), 0},
            sf::Vector2i{static_cast<int>(curr_health_.getLocalBounds().width),
                         static_cast<int>(curr_health_.getLocalBounds().height)}});
    }



}

void HealthBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(curr_health_, states);
}

void HealthBar::setPosition(float x, float y)
{
    AbstractDrawableObject::setPosition(x, y);
    curr_health_.setPosition(x, y);
}

void HealthBar::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);
    curr_health_.setPosition(pos);
}