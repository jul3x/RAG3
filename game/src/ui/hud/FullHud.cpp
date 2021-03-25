//
// Created by jul3x on 08.04.20.
//

#include <functional>

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/hud/FullHud.h>
#include <common/ui/UserInterface.h>


BackpackHud::BackpackHud(UserInterface* ui, Player* player, const sf::Vector2f& pos, int x, int y) :
        player_(player), ui_(ui)
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

            tooltips_.emplace_back(ui->getTheme(), pos + sf::Vector2f{static_cast<float>(j), static_cast<float>(i)}
                                                         * CONF<float>("graphics/backpack_placeholder_diff")
                                                   - CONF<sf::Vector2f>("graphics/backpack_placeholder_size") / 2.0f);
        }
    }

    for (auto& tooltip : tooltips_)
    {
        tooltip.bindFunction([this, &tooltip]() { this->clickPlaceholder(tooltip); });
        tooltip.bindGui(ui->getGui());
        tooltip.show(true);
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
    for (auto& special : player_->getBackpack())
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
                        player_->upgradeWeapon(w, s);
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

    for (auto& special : player_->getBackpack())
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

    for (auto& special : player_->getBackpack())
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
    for (auto& weapon : player_->getWeapons())
    {
        if (weapon->getId() != "null")
        {
            weapons_.emplace_back(AbstractDrawableObject{placeholders_[i].getPosition(),
                                                         RMGET<sf::Vector2f>("specials", weapon->getId(), "size") *
                                                         CONF<float>("graphics/global_zoom"),
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

void BackpackHud::show(bool hide)
{
    for (auto& tooltip : tooltips_)
    {
        tooltip.show(hide);
    }
}

void BackpackHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto& placeholder : placeholders_)
    {
        target.draw(placeholder, states);
    }

    size_t i = 0;
    for (auto& special : player_->getBackpack())
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


SkillsHud::SkillsHud(UserInterface* ui, Player* player, const sf::Vector2f& pos) :
        points_text_("", RM.getFont(), CONF<float>("graphics/skills_text_size")),
        player_(player)
{
    int i = 0;
    for (const auto& line : CONF<j3x::List>("graphics/skills_lines"))
    {
        auto line_pos = j3x::getObj<sf::Vector2f>(line);
        lines_.emplace_back(sf::Color::White, 3.0f);
        lines_.back().setStart(pos + line_pos);
        lines_.back().setEnd(pos + line_pos);
        lines_.back().setThickness(CONF<float>("graphics/lines_thickness"));
        lines_.back().forceSet();

        texts_.emplace_back(texts_placeholders_[i], RM.getFont(), CONF<float>("graphics/skills_text_size"));
        texts_.back().setFillColor(sf::Color::White);
        texts_.back().setPosition(lines_.back().getStart() + CONF<sf::Vector2f>("graphics/skills_text_offset"));

        auto button_pos = texts_.back().getPosition() + sf::Vector2f(texts_.back().getLocalBounds().width, 0)
                          + CONF<sf::Vector2f>("graphics/skills_button_offset");
        buttons_.emplace_back(tgui::Button::create("+"));
        buttons_.back()->setRenderer(ui->getTheme()->getRenderer("AddSkillButton"));
        buttons_.back()->setPosition(button_pos);
        buttons_.back()->setSize(CONF<sf::Vector2f>("graphics/skills_button_size"));
        buttons_.back()->setVisible(false);
        buttons_.back()->connect("pressed", [this](Player::Skills skill) {
            if (!player_->addSkill(skill))
            {
                for (auto& button : buttons_)
                    button->setVisible(false);
            }
        }, skills_[i]);
        ui->getGui()->add(buttons_.back());

        ++i;
        if (i >= 4)
            break;
    }

    points_text_.setFillColor(sf::Color::White);
    points_text_.setPosition(pos + CONF<sf::Vector2f>("graphics/skill_points_offset"));
}

void SkillsHud::update(float time_elapsed)
{
    for (auto& line : lines_)
    {
        line.update(time_elapsed);
    }

    size_t i = 0;
    for (auto& text : texts_)
    {
        text.setString(texts_placeholders_[i] + std::to_string(player_->getSkill(skills_[i])));
        ++i;
    }

    points_text_.setString("Points: " + std::to_string(player_->getSkillPoints()));
}

void SkillsHud::show(bool hide)
{
    if (!hide)
    {
        for (auto& line : lines_)
        {
            line.setEnd(line.getStart() - sf::Vector2f{CONF<float>("graphics/skills_lines_length"), 0.0f});
        }
    }
    else
    {
        for (auto& line : lines_)
        {
            line.setEnd(line.getStart());
        }
    }

    for (auto& button : buttons_)
    {
        if (player_->getSkillPoints() > 0)
        {
            if (hide)
            {
                button->hideWithEffect(tgui::ShowAnimationType::Fade,
                                       sf::seconds(CONF<float>("graphics/full_hud_show_duration") / 2.0f));
            }
            else
            {
                button->showWithEffect(tgui::ShowAnimationType::Fade,
                                       sf::seconds(CONF<float>("graphics/full_hud_show_duration")));
            }
        }
    }
}

void SkillsHud::setColor(const sf::Color& color)
{
    for (auto& line : lines_)
    {
        line.setColor(color);
    }

    for (auto& text : texts_)
    {
        text.setFillColor(color);
    }

    points_text_.setFillColor(color);
}

void SkillsHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto& line : lines_)
    {
        target.draw(line, states);
    }

    for (auto& text : texts_)
    {
        target.draw(text, states);
    }

    target.draw(points_text_, states);
}


FullHud::FullHud(UserInterface* ui, Player* player, const sf::Vector2f& size) :
        show_(false),
        bg_color_(sf::Color(0, 0, 0, 0)),
        bg_(size),
        player_(size / 2.0f + CONF<sf::Vector2f>("graphics/huge_player_offset"),
                CONF<sf::Vector2f>("graphics/huge_player_size"),
                &RM.getTexture("huge_player")),
        backpack_hud_(ui, player,
                      size / 2.0f + CONF<sf::Vector2f>("graphics/backpack_offset"),
                      CONF<int>("graphics/backpack_placeholders_x"),
                      CONF<int>("graphics/backpack_placeholders_y")),
        skills_hud_(ui, player, size / 2.0f + CONF<sf::Vector2f>("graphics/skills_offset")),
        time_elapsed_(0.0f)
{
    bg_.setPosition(0.0f, 0.0f);
    bg_.setFillColor(bg_color_);
    player_.setColor(255, 255, 255, bg_color_.a);
    backpack_hud_.setOpacity(bg_color_.a);

    auto label = tgui::Button::create();
    label->setRenderer(ui->getTheme()->getRenderer("MenuButton"));
    label->setText("Back to menu");
    label->setInheritedFont(RM.getFont("default"));
    label->setTextSize(CONF<float>("graphics/menu_button_text_size"));
    label->setPosition(sf::Vector2f(CONF<int>("graphics/window_width_px") / 2.0f - label->getFullSize().x / 2.0f,
                                    CONF<int>("graphics/window_height_px")) +
                       CONF<sf::Vector2f>("graphics/back_to_menu_pos"));
    label->setVisible(false);
    label->connect("pressed", [ui]() { ui->openMenu(); });

    ui->getGui()->add(label);
    buttons_.emplace_back(label);
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

    if (show_ || time_elapsed_ > 0)
    {
        skills_hud_.update(time_elapsed);
        backpack_hud_.update(time_elapsed);
    }

    bg_.setFillColor(bg_color_);
    player_.setColor(255, 255, 255,
                     static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    backpack_hud_.setOpacity(static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    skills_hud_.setColor(
            {255, 255, 255, static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity"))});

    time_elapsed_ = std::max(0.0f, time_elapsed_ - time_elapsed);
}

void FullHud::show(bool show)
{
    if (show != show_)
    {
        for (auto& button : buttons_)
        {
            if (show)
                button->showWithEffect(tgui::ShowAnimationType::Fade,
                                       sf::seconds(CONF<float>("graphics/full_hud_show_duration")));
            else
                button->hideWithEffect(tgui::ShowAnimationType::Fade,
                                       sf::seconds(CONF<float>("graphics/full_hud_show_duration") / 2.0f));
        }

        time_elapsed_ = CONF<float>("graphics/full_hud_show_duration");
        show_ = show;
    }

    skills_hud_.show(!show_);
    backpack_hud_.show(!show_);
}

void FullHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (show_ || time_elapsed_ > 0.0f)
    {
        target.draw(bg_, states);
        target.draw(player_, states);
        target.draw(backpack_hud_, states);
        target.draw(skills_hud_, states);
    }
}

bool FullHud::isShow() const
{
    return show_;
}
