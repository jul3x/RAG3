//
// Created by jul3x on 03.11.19.
//

#include <iostream>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <common/ShootingWeapon.h>
#include <ui/WeaponsBar.h>


WeaponsBar::WeaponsBar(const sf::Vector2f& position) :
        AbstractDrawableObject(position, {2.0f, 2.0f}, nullptr),
        weapons_positions_({sf::Vector2f(-461.0f, -140.0f), sf::Vector2f(-405.0f, -278.0f),
                            sf::Vector2f(-286.0f, -366.0f), sf::Vector2f(-149.0f, -410.0f)})
{
    for (int i = 0; i < SLOTS_; ++i)
    {
        ammo_.emplace_back("0", RM.getFont(), 16 * CFG.getFloat("graphics/user_interface_zoom"));
        ammo_.at(i).setFillColor(sf::Color(CFG.getInt("graphics/font_color")));

        ammo_quantity_.emplace_back(CFG.getFloat("graphics/inertial_states_change_speed"));
    }
}

void WeaponsBar::update(const std::vector<std::shared_ptr<AbstractWeapon>>& weapons, int curr_weapon, float time_elapsed)
{
    weapons_.clear();

    for (auto& quantity : ammo_quantity_)
    {
        quantity.update(time_elapsed);
    }

    auto base_position = this->getPosition();
    for (size_t i = 0; i < SLOTS_; ++i)
    {
        auto mod_i = (curr_weapon + i) % weapons.size();
        auto weapon_cast = dynamic_cast<ShootingWeapon*>(weapons.at(mod_i).get());
        auto size = i == 0 ? sf::Vector2f(CHOSEN_WEAPON_SIZE_X_, CHOSEN_WEAPON_SIZE_Y_) :
                    sf::Vector2f(WEAPON_SIZE_X_, WEAPON_SIZE_Y_);

        if (weapon_cast != nullptr)
        {
            auto weapon_pos = base_position + weapons_positions_.at(i) * CFG.getFloat("graphics/user_interface_zoom");
            weapons_.emplace_back(weapon_pos, size * CFG.getFloat("graphics/user_interface_zoom"),
                                  &RM.getTexture("specials/" + weapon_cast->getName()));

            ammo_quantity_.at(i).setState(weapon_cast->getAmmunition());

            ammo_.at(i).setString(std::to_string(static_cast<int>(std::round(ammo_quantity_.at(i).getState()))));

            sf::FloatRect text_rect = ammo_.at(i).getLocalBounds();
            ammo_.at(i).setOrigin(text_rect.left + text_rect.width / 2.0f,
                                  text_rect.top + text_rect.height / 2.0f);
            ammo_.at(i).setPosition(weapon_pos + sf::Vector2f{0.0f, -30.0f * CFG.getFloat("graphics/user_interface_zoom")});
        }
        else
        {
            ammo_.at(i).setString("");
        }
    }
}

void WeaponsBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& weapon : weapons_)
    {
        target.draw(weapon, states);
    }

    for (const auto& ammo_text : ammo_)
    {
        target.draw(ammo_text, states);
    }
}
