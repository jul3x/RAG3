//
// Created by jprolejko on 03.11.19.
//

#include <iostream>

#include <system/ResourceManager.h>
#include <objects/ShootingWeapon.h>
#include <objects/WeaponsBar.h>


WeaponsBar::WeaponsBar(const sf::Vector2f &position) :
        current_highlight_(position, {WEAPON_SIZE_X_, WEAPON_SIZE_Y_}, "current_weapon_highlight"),
        AbstractDrawableObject(position, {SIZE_X_, SIZE_Y_}, "weapons_bar") {
    for (int i = 0; i < SLOTS_; ++i)
    {
        ammo_.emplace_back("0", ResourceManager::getInstance().getFont(), 20);
        ammo_.at(i).setFillColor(sf::Color::White);
    }
}

void WeaponsBar::updateWeaponsList(const std::vector<std::unique_ptr<AbstractWeapon>> &weapons) {
    weapons_.clear();

    int i = 0;
    auto base_position = this->getPosition() + sf::Vector2f{WEAPON_SIZE_X_ / 2.0f, 0.0f}
        - SLOTS_ / 2.0f * sf::Vector2f{WEAPON_SIZE_X_, 0.0f};
    for (const auto &weapon : weapons)
    {
        auto weapon_cast = dynamic_cast<ShootingWeapon*>(weapon.get());

        if (weapon_cast != nullptr)
        {
            weapons_.push_back({base_position + static_cast<float>(i) * sf::Vector2f{WEAPON_SIZE_X_, 0.0f},
                               {WEAPON_SIZE_X_, WEAPON_SIZE_Y_}, "weapon_mini_" + weapon_cast->getName()});

            ammo_.at(i).setString(std::to_string(weapon_cast->getAmunition()));
            ammo_.at(i).setPosition({base_position + static_cast<float>(i) * sf::Vector2f{WEAPON_SIZE_X_, 0.0f}});
        }
        else
        {
            ammo_.at(i).setString("");
        }
        

        ++i;
    }
}

void WeaponsBar::updateCurrentWeapon(int curr_weapon) {
    auto base_position = this->getPosition() + sf::Vector2f{WEAPON_SIZE_X_ / 2.0f, 0.0f}
        - SLOTS_ / 2.0f * sf::Vector2f{WEAPON_SIZE_X_, 0.0f};
    current_highlight_.setPosition(base_position + static_cast<float>(curr_weapon) * sf::Vector2f{WEAPON_SIZE_X_, 0.0f});
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
