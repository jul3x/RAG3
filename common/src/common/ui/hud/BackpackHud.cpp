//
// Created by jul3x on 16.10.21.
//

#include <functional>

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/hud/BackpackHud.h>
#include <common/ui/UserInterface.h>
#include <common/Framework.h>


BackpackHud::BackpackHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos, int x, int y) :
        framework_(framework), ui_(ui)
{
    for (int i = 0; i < y; ++i)
    {
        for (int j = 0; j < x; ++j)
        {
            placeholders_.emplace_back(pos + sf::Vector2f{static_cast<float>(j), static_cast<float>(i)} *
                                             CONF<float>("graphics/backpack_placeholder_diff"),
                                       CONF<sf::Vector2f>("graphics/backpack_placeholder_size"),
                                       &RM.getTexture("backpack_place"));
            numbers_.emplace_back("", RM.getFont(), CONF<float>("graphics/backpack_text_size"));

            tooltips_.emplace_back(ui->getFramework(), ui->getTheme(),
                                   pos + sf::Vector2f{static_cast<float>(j), static_cast<float>(i)}
                                         * CONF<float>("graphics/backpack_placeholder_diff")
                                   - CONF<sf::Vector2f>("graphics/backpack_placeholder_size") / 2.0f,
                                   CONF<sf::Vector2f>("graphics/backpack_placeholder_size"));
        }
    }

    for (auto& tooltip : tooltips_)
    {
        tooltip.bindFunction([this, &tooltip]() { this->clickPlaceholder(tooltip); });
        tooltip.bindGui(ui->getGui());
        tooltip.show(false);
    }
}

void BackpackHud::clickPlaceholder(Tooltip& tooltip)
{
    tooltip.setActive(true);
    auto& active = this->getActiveTooltips();

    if (active.size() >= 2)
    {
        this->combineBackpackItems(active[0], active[1]);
        this->resetActiveTooltips();
    }
}

void BackpackHud::combineBackpackItems(size_t first, size_t second)
{
    size_t i = 0;
    std::string special_id{}, weapon_id{};
    auto player = framework_->getPlayer();

    for (auto& special : player->getBackpack())
    {
        if (i == first)
            special_id = special.first.getId();
        if (i == second)
            special_id = special.first.getId();
        ++i;
    }

    for (auto& weapon : weapons_)
    {
        if (i == first)
            weapon_id = weapon.second;
        if (i == second)
            weapon_id = weapon.second;
        ++i;
    }

    if (!special_id.empty() && !weapon_id.empty())
    {
        auto& upgrade = RMGET<j3x::List>("specials", special_id, "can_upgrade", true);
        bool can_upgrade = false;
        for (auto& weapon_to_upgrade : upgrade)
        {
            if (j3x::getObj<std::string>(weapon_to_upgrade) == weapon_id)
            {
                can_upgrade = true;
            }
        }

        if (can_upgrade)
        {
            ui_->spawnAcceptWindow(
                    "Do you want to upgrade \"" +
                    RMGET<std::string>("specials", weapon_id, "tooltip_header") + "\" with \"" +
                    RMGET<std::string>("specials", special_id, "tooltip_header") + "\"?",
                    std::bind([this](const std::string& w, const std::string& s) {
                        framework_->getPlayer()->upgradeWeapon(w, s);
                        framework_->spawnSound(RM.getSound("ui_upgrade"));
                    }, weapon_id, special_id));
        }
    }
}

void BackpackHud::setOpacity(sf::Uint8 a)
{
    for (auto& placeholder : placeholders_)
    {
        placeholder.setColor(255, 255, 255, a);
    }

    for (auto& number : numbers_)
    {
        number.setFillColor({255, 255, 255, a});
    }

    for (auto& special : framework_->getPlayer()->getBackpack())
    {
        special.first.setColor(255, 255, 255, a);
    }

    for (auto& weapon : weapons_)
    {
        weapon.first.setColor(255, 255, 255, a);
    }
}

const std::vector<size_t>& BackpackHud::getActiveTooltips()
{
    static std::vector<size_t> result;
    result.clear();

    size_t i = 0;
    for (auto& tooltip : tooltips_)
    {
        if (tooltip.isActive())
            result.emplace_back(i);
        ++i;
    }

    return result;
}

void BackpackHud::resetActiveTooltips()
{
    for (auto& tooltip : tooltips_)
        tooltip.setActive(false);
}

void BackpackHud::update(float time_elapsed)
{
    size_t i = 0;
    auto player = framework_->getPlayer();
    for (auto& special : player->getBackpack())
    {

        special.first.setPosition(placeholders_[i].getPosition());

        tooltips_[i].bindText(RMGET<std::string>("specials", special.first.getId(), "tooltip_header"),
                              RMGET<std::string>("specials", special.first.getId(), "tooltip"));

        if (special.second > 1)
        {
            numbers_[i].setString(std::to_string(special.second));
            numbers_[i]
                    .setPosition(placeholders_[i].getPosition() + CONF<sf::Vector2f>("graphics/backpack_number_diff"));
        }

        ++i;
    }

    weapons_.clear();
    for (auto& weapon : player->getWeapons())
    {
        if (weapon->getId() != "null")
        {
            if (!RMGET<bool>("specials", weapon->getId(), "is_drawable"))
                continue;
            
            weapons_.emplace_back(AbstractDrawableObject{placeholders_[i].getPosition(),
                                                         RMGET<sf::Vector2f>("specials", weapon->getId(), "size") *
                                                         CONF<float>("graphics/GLOBAL_ZOOM"),
                                                         &RM.getTexture("specials/" + weapon->getId())},
                                  weapon->getId());

            auto tooltip_text = RMGET<std::string>("specials", weapon->getId(), "tooltip");
            const auto& upgrades = weapon->getUpgrades();
            if (!upgrades.empty())
            {
                tooltip_text += "\n\nUpgrades:\n";

                for (const auto& upgrade : upgrades)
                {
                    tooltip_text += "- " + RMGET<std::string>("specials", upgrade, "tooltip_header") + "\n";
                }
                tooltip_text.pop_back();
            }

            tooltips_[i].bindText(RMGET<std::string>("specials", weapon->getId(), "tooltip_header"),
                                  tooltip_text);
            ++i;
        }
    }

    for (; i < tooltips_.size(); ++i)
    {
        tooltips_[i].bindText("", "");
    }
}

void BackpackHud::show(bool show)
{
    for (auto& tooltip : tooltips_)
    {
        tooltip.show(show);
    }
}

void BackpackHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto& placeholder : placeholders_)
    {
        target.draw(placeholder, states);
    }

    size_t i = 0;
    for (auto& special : framework_->getPlayer()->getBackpack())
    {
        target.draw(special.first, states);
        if (special.second > 1)
        {
            target.draw(numbers_[i]);
        }

        ++i;
    }

    for (auto& weapon : weapons_)
    {
        target.draw(weapon.first);
    }
}
