//
// Created by jul3x on 29.03.2021.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/characters/LifeBar.h>

using namespace r3e;


LifeBar::LifeBar() : LifeBar("", 1.0f)
{

}

LifeBar::LifeBar(const std::string& name, float max_health) :
        max_health_(max_health), health_(CONF<float>("graphics/inertial_states_change_speed")),
        line_(sf::TriangleStrip, 4),
        name_(name, RM.getFont(), CONF<float>("graphics/player_names_text_size"))
{
    health_.setState(max_health);
    health_.setForcedState(max_health);

    sf::Color color(CONF<int>("graphics/health_bar_color"));
    line_[0].color = color;
    line_[1].color = color;
    line_[2].color = color;
    line_[3].color = color;
    auto rect = name_.getGlobalBounds();
    name_.setOrigin(rect.width / 2.0f, rect.height / 2.0f);
}

void LifeBar::update(float time_elapsed)
{
    health_.update(time_elapsed);

    float width = health_.getState() / max_health_ * CONF <float>("graphics/health_bar_max_width");
    float thickness = CONF<float>("graphics/health_bar_thickness");
    line_[0].position = pos_ - sf::Vector2f{width / 2.0f, thickness / 2.0f};
    line_[1].position = pos_ - sf::Vector2f{width / 2.0f, -thickness / 2.0f};
    line_[2].position = pos_ + sf::Vector2f{width / 2.0f, -thickness / 2.0f};
    line_[3].position = pos_ + sf::Vector2f{width / 2.0f, thickness / 2.0f};
    name_.setPosition(pos_ + sf::Vector2f(0.0, CONF<float>("graphics/health_bar_name_offset")));
}

void LifeBar::setMaxHealth(float max_health)
{
    max_health_ = max_health;
}

void LifeBar::setHealth(float health)
{
    health_.setState(health);
}

void LifeBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(line_, states);
    target.draw(name_, states);
}

void LifeBar::setPosition(const sf::Vector2f &pos)
{
    pos_ = pos + sf::Vector2f(0.0, CONF<float>("graphics/health_bar_offset"));
}
