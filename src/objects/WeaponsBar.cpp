//
// Created by jprolejko on 03.11.19.
//

#include <objects/WeaponsBar.h>


WeaponsBar::WeaponsBar(const sf::Vector2f &position) :
        AbstractDrawableObject(position, {SIZE_X_, SIZE_Y_}, "weapons_bar") {}

void WeaponsBar::updateWeaponsList(const std::vector<Weapon> &weapons) {
    weapons_.clear();

    int i = -SLOTS_ / 2;
    auto base_position = this->getPosition() + sf::Vector2f{WEAPON_SIZE_X_ / 2.0f, 0.0f};
    for (const auto &weapon : weapons)
    {
        weapons_.push_back({base_position + static_cast<float>(i) * sf::Vector2f{WEAPON_SIZE_X_, 0.0f},
                           {WEAPON_SIZE_X_, WEAPON_SIZE_Y_}, "weapon_mini_" + weapon.getName()});
        ++i;
    }
}

void WeaponsBar::updateCurrentWeapon(int curr_weapon) {
// TODO

}

void WeaponsBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape_, states);

    for (const auto &weapon : weapons_)
    {
        target.draw(weapon, states);
    }
}
