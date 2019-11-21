//
// Created by jprolejko on 03.11.19.
//

#include <objects/WeaponsBar.h>


WeaponsBar::WeaponsBar(const sf::Vector2f &position) :
        current_highlight_(position, {WEAPON_SIZE_X_, WEAPON_SIZE_Y_}, "current_weapon_highlight"),
        AbstractDrawableObject(position, {SIZE_X_, SIZE_Y_}, "weapons_bar") {}

void WeaponsBar::updateWeaponsList(const std::vector<std::unique_ptr<Weapon>> &weapons) {
    weapons_.clear();

    int i = 0;
    static auto base_position = this->getPosition() + sf::Vector2f{WEAPON_SIZE_X_ / 2.0f, 0.0f}
        - SLOTS_ / 2.0f * sf::Vector2f{WEAPON_SIZE_X_, 0.0f};
    for (const auto &weapon : weapons)
    {
        if (weapon->getName() != "noweapon")
        {
            weapons_.push_back({base_position + static_cast<float>(i) * sf::Vector2f{WEAPON_SIZE_X_, 0.0f},
                               {WEAPON_SIZE_X_, WEAPON_SIZE_Y_}, "weapon_mini_" + weapon->getName()});
        }

        ++i;
    }
}

void WeaponsBar::updateCurrentWeapon(int curr_weapon) {
    static auto base_position = this->getPosition() + sf::Vector2f{WEAPON_SIZE_X_ / 2.0f, 0.0f}
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
}
