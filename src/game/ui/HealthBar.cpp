//
// Created by jul3x on 21.11.19.
//

#include <game/misc/ResourceManager.h>
#include <engine/system/Config.h>
#include <game/ui/HealthBar.h>


HealthBar::HealthBar(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {HEALTH_SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                                HEALTH_SIZE_Y_ * CFG.getFloat("user_interface_zoom")},
                               &RM.getTexture("health_bar")),
        health_text_("Health: 100%", RM.getFont(),
                     16 * CFG.getFloat("user_interface_zoom"))
{
    health_text_.setFillColor(sf::Color(CFG.getInt("font_color")));
    health_text_.setPosition(
            this->getPosition() - sf::Vector2f{HEALTH_SIZE_X_ / 2.0f * CFG.getFloat("user_interface_zoom"),
                                               30.0f * CFG.getFloat("user_interface_zoom")});
}

void HealthBar::setMaxHealth(int max_health)
{
    this->max_health_ = max_health;
}

void HealthBar::updateHealth(int health)
{
    int percentage = (100 * health) / max_health_;

    shape_.setSize({HEALTH_SIZE_X_ * percentage / 100.0f * CFG.getFloat("user_interface_zoom"),
                    HEALTH_SIZE_Y_ * CFG.getFloat("user_interface_zoom")});
    health_text_.setString("Health: " + std::to_string(percentage) + "%");
}

void HealthBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(health_text_, states);
}
