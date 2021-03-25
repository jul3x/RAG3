//
// Created by jul3x on 06.10.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/hud/LevelHud.h>


LevelHud::LevelHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("level_hud")),
        level_(CONF<float>("graphics/inertial_states_change_speed")),
        exp_(CONF<float>("graphics/inertial_states_change_speed")),
        level_text_("Level: 0", RM.getFont(), CONF<float>("graphics/level_text_size")),
        exp_text_("Experience: 0", RM.getFont(), CONF<float>("graphics/stats_text_size"))
{
    level_text_.setFillColor(sf::Color::White);
    level_text_.setPosition(LEVEL_X_ * CONF<float>("graphics/user_interface_zoom"),
                            LEVEL_Y_ * CONF<float>("graphics/user_interface_zoom"));
    level_text_.setOrigin(-position);

    exp_text_.setFillColor(sf::Color::White);
    exp_text_.setPosition(EXP_X_ * CONF<float>("graphics/user_interface_zoom"),
                          EXP_Y_ * CONF<float>("graphics/user_interface_zoom"));
    exp_text_.setOrigin(-position);

    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom") / 2.0f, 0.0f});
}

void LevelHud::update(int level, int exp, float time_elapsed)
{
    level_.setState(level);
    exp_.setState(exp);

    level_.update(time_elapsed);
    exp_.update(time_elapsed);

    auto level_str = std::to_string(static_cast<int>(std::round(level_.getState())));
    auto exp_str = std::to_string(static_cast<int>(std::round(exp_.getState())));

    level_text_.setString("Level: " + level_str);
    exp_text_.setString("Experience: " + exp_str);
}

void LevelHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(level_text_, states);
    target.draw(exp_text_, states);
}
