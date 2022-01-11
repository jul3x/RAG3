//
// Created by jul3x on 11.01.22.
//

#include <common/Framework.h>

#include <ui/GameFullHud.h>


GameFullHud::GameFullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size) :
        FullHud(ui, framework, size),
        player_(size / 2.0f + CONF<sf::Vector2f>("graphics/huge_player_offset"),
                CONF<sf::Vector2f>("graphics/huge_player_size"),
                &RM.getTexture("huge_player")),
        backpack_hud_(ui, framework,
                      size / 2.0f + CONF<sf::Vector2f>("graphics/backpack_offset"),
                      CONF<int>("graphics/backpack_placeholders_x"),
                      CONF<int>("graphics/backpack_placeholders_y")),
        skills_hud_(ui, framework, size / 2.0f + CONF<sf::Vector2f>("graphics/skills_offset")),
        framework_(framework)
{
    player_.setColor(255, 255, 255, bg_color_.a);
    backpack_hud_.setOpacity(bg_color_.a);

    auto button_size = CONF<float>("graphics/menu_button_text_size");
    auto center_top = sf::Vector2f(CONF<int>("graphics/window_width_px") / 2.0f, 0);
    auto show_duration = CONF<float>("graphics/full_hud_show_duration");
    buttons_.emplace_back(std::make_unique<TextButton>(framework, "Respawn",
                                                       center_top +
                                                       CONF<sf::Vector2f>("graphics/respawn_button_pos"),
                                                       button_size, show_duration));
    buttons_.back()->bindFunction([this, ui]() {
        this->framework_->respawn("");
        ui->clearWindows();
        this->show(false);
    });
}

void GameFullHud::update(float time_elapsed)
{
    FullHud::update(time_elapsed);

    if (!framework_->isNormalGameplay())
        return;

    if (show_ || time_elapsed_ > 0)
    {
        skills_hud_.update(time_elapsed);
        backpack_hud_.update(time_elapsed);
    }

    player_.setColor(255, 255, 255,
                     static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    backpack_hud_.setOpacity(static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    skills_hud_.setColor(
            {255, 255, 255, static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity"))});
}

void GameFullHud::show(bool show)
{
    FullHud::show(show);

    if (!framework_->isNormalGameplay())
        return;

    skills_hud_.show(show_);
    backpack_hud_.show(show_);
}

void GameFullHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (show_ || time_elapsed_ > 0.0f)
    {
        target.draw(bg_, states);

        if (!framework_->isNormalGameplay())
            return;

        target.draw(player_, states);
        target.draw(backpack_hud_, states);
        target.draw(skills_hud_, states);
    }
}
