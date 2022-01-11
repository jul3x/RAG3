//
// Created by jul3x on 08.04.20.
//

#include <functional>

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/hud/FullHud.h>
#include <common/ui/UserInterface.h>
#include <common/Framework.h>


FullHud::FullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size) :
        show_(false),
        bg_color_(sf::Color(0, 0, 0, 0)),
        bg_(size),
        time_elapsed_(0.0f)
{
    bg_.setPosition(0.0f, 0.0f);
    bg_.setFillColor(bg_color_);

    auto button_size = CONF<float>("graphics/menu_button_text_size");
    auto center_bottom =
            sf::Vector2f(CONF<int>("graphics/window_width_px") / 2.0f, CONF<int>("graphics/window_height_px"));
    auto show_duration = CONF<float>("graphics/full_hud_show_duration");
    buttons_.emplace_back(std::make_unique<TextButton>(framework, "Back to menu",
                                                       center_bottom +
                                                       CONF<sf::Vector2f>("graphics/back_to_menu_button_pos"),
                                                       button_size, show_duration));
    buttons_.back()->bindFunction([ui]() { ui->openMenu(); });
}

void FullHud::update(float time_elapsed)
{
    static const auto opacity_factor =
            CONF<int>("graphics/full_hud_max_opacity") / CONF<float>("graphics/full_hud_show_duration");

    auto delta = static_cast<int>(time_elapsed * opacity_factor);
    if (!show_)
    {
        bg_color_.a = static_cast<sf::Uint8>(std::min(std::max(bg_color_.a - delta, 0),
                                                      CONF<int>("graphics/full_hud_max_opacity")));
    }
    else
    {
        bg_color_.a = static_cast<sf::Uint8>(std::min(std::max(bg_color_.a + delta, 0),
                                                      CONF<int>("graphics/full_hud_max_opacity")));
    }

    bg_.setFillColor(bg_color_);

    time_elapsed_ = std::max(0.0f, time_elapsed_ - time_elapsed);
}

void FullHud::show(bool show)
{
    if (show != show_)
    {
        for (auto& button : buttons_)
        {
            button->show(show);
        }

        time_elapsed_ = CONF<float>("graphics/full_hud_show_duration");
        show_ = show;
    }
}

void FullHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (show_ || time_elapsed_ > 0.0f)
    {
        target.draw(bg_, states);
    }
}

bool FullHud::isShow() const
{
    return show_;
}