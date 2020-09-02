//
// Created by jul3x on 08.04.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/StatsHud.h>


StatsHud::StatsHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CFG.get<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CFG.get<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("stats_hud")),
        deaths_(CFG.get<float>("graphics/inertial_states_change_speed")),
        crystals_(CFG.get<float>("graphics/inertial_states_change_speed")),
        deaths_text_("0000000", RM.getFont(), CFG.get<int>("graphics/stats_text_size") * CFG.get<float>("graphics/user_interface_zoom")),
        crystals_text_("0000000", RM.getFont(), CFG.get<int>("graphics/stats_text_size") * CFG.get<float>("graphics/user_interface_zoom"))
{
    deaths_text_.setFillColor(sf::Color::White);
    deaths_text_.setPosition(DEATHS_X_ * CFG.get<float>("graphics/user_interface_zoom"),
            DEATHS_Y_ * CFG.get<float>("graphics/user_interface_zoom"));

    crystals_text_.setFillColor(sf::Color::White);
    crystals_text_.setPosition(CRYSTALS_X_ * CFG.get<float>("graphics/user_interface_zoom"),
            CRYSTALS_Y_ * CFG.get<float>("graphics/user_interface_zoom"));

    this->changeOrigin({0.0f, 0.0f});
}

void StatsHud::update(int deaths, int crystals, float time_elapsed)
{
    deaths_.setState(deaths);
    crystals_.setState(crystals);

    deaths_.update(time_elapsed);
    crystals_.update(time_elapsed);

    auto deaths_str = std::to_string(static_cast<int>(std::round(deaths_.getState())));
    auto crystals_str = std::to_string(static_cast<int>(std::round(crystals_.getState())));

    deaths_text_.setString(std::string(7 - deaths_str.length(), '0') + deaths_str);
    crystals_text_.setString(std::string(7 - crystals_str.length(), '0') + crystals_str);
}

void StatsHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(deaths_text_, states);
    target.draw(crystals_text_, states);
}
