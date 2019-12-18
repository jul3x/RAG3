//
// Created by jprolejko on 03.11.19.
//

#include <iostream>

#include <system/ResourceManager.h>
#include <system/Config.h>
#include <objects/ShootingWeapon.h>
#include <objects/WeaponsBar.h>


WeaponsBar::WeaponsBar(const sf::Vector2f &position) :
        current_highlight_(position,
                           {WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                            WEAPON_SIZE_Y_ * CFG.getFloat("user_interface_zoom")}, "current_weapon_highlight"),
        AbstractDrawableObject(position,
                               {SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                                SIZE_Y_* CFG.getFloat("user_interface_zoom")}, "weapons_bar") {
    for (int i = 0; i < SLOTS_; ++i)
    {
        ammo_.emplace_back("0", ResourceManager::getInstance().getFont(), 16 * CFG.getFloat("user_interface_zoom"));
        ammo_.at(i).setFillColor(sf::Color(CFG.getInt("font_color")));
    }
}

void WeaponsBar::updateWeaponsList(const std::vector<std::unique_ptr<AbstractWeapon>> &weapons) {
    weapons_.clear();

    int i = 0;
    auto base_position = this->getPosition() +
            sf::Vector2f{WEAPON_SIZE_X_ / 2.0f * CFG.getFloat("user_interface_zoom"), 0.0f} -
            SLOTS_ / 2.0f * sf::Vector2f{WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"), 0.0f};
    for (const auto &weapon : weapons)
    {
        auto weapon_cast = dynamic_cast<ShootingWeapon*>(weapon.get());

        if (weapon_cast != nullptr)
        {
            auto weapon_pos = base_position + static_cast<float>(i) *
                    sf::Vector2f{WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"), 0.0f};
            weapons_.push_back({weapon_pos, {WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                                             WEAPON_SIZE_Y_ * CFG.getFloat("user_interface_zoom")},
                                "weapon_mini_" + weapon_cast->getName()});

            ammo_.at(i).setString(std::to_string(weapon_cast->getAmunition()));
            sf::FloatRect text_rect = ammo_.at(i).getLocalBounds();
            ammo_.at(i).setOrigin(text_rect.left + text_rect.width / 2.0f,
                                  text_rect.top + text_rect.height / 2.0f);
            ammo_.at(i).setPosition(weapon_pos + sf::Vector2f{0.0f, -20.0f * CFG.getFloat("user_interface_zoom")});
        }
        else
        {
            ammo_.at(i).setString("");
        }

        ++i;
    }
}

void WeaponsBar::updateCurrentWeapon(int curr_weapon) {
    auto base_position = this->getPosition() +
            sf::Vector2f{WEAPON_SIZE_X_ / 2.0f * CFG.getFloat("user_interface_zoom"), 0.0f} -
            SLOTS_ / 2.0f * sf::Vector2f{WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"), 0.0f};
    current_highlight_.setPosition(base_position + static_cast<float>(curr_weapon) *
            sf::Vector2f{WEAPON_SIZE_X_ * CFG.getFloat("user_interface_zoom"), 0.0f});
}

void WeaponsBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape_, states);
    target.draw(current_highlight_, states);

    for (const auto &weapon : weapons_)
    {
        target.draw(weapon, states);
    }

    for (const auto &ammo_text : ammo_)
    {
        target.draw(ammo_text, states);
    }
}
