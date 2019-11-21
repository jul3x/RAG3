//
// Created by jprolejko on 21.11.19.
//

#include <system/ResourceManager.h>
#include <system/Config.h>
#include <objects/HealthBar.h>


HealthBar::HealthBar(const sf::Vector2f &position) :
        AbstractDrawableObject(position, {HEALTH_SIZE_X_, HEALTH_SIZE_Y_}, "health_bar"),
        health_text_("Health: 100%", ResourceManager::getInstance().getFont(), 16) {
    health_text_.setFillColor(sf::Color(CFG.getInt("font_color")));
    health_text_.setPosition(this->getPosition() - sf::Vector2f{HEALTH_SIZE_X_ / 2.0f, 30.0f});
}

void HealthBar::setMaxHealth(int max_health) {
    this->max_health_ = max_health;
}

void HealthBar::updateHealth(int health) {
    int percentage = (100 * health) / max_health_;

    shape_.setSize({HEALTH_SIZE_X_ * percentage / 100.0f, HEALTH_SIZE_Y_});
    health_text_.setString("Health: " + std::to_string(percentage) + "%");
}

void HealthBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape_, states);
    target.draw(health_text_, states);
}
