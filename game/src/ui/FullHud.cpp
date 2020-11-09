//
// Created by jul3x on 08.04.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/FullHud.h>
#include <Game.h>


BackpackHud::BackpackHud(const sf::Vector2f& pos, int x, int y)
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

    for (auto& special : Game::get().getPlayer().getBackpack())
    {
        special.first.setColor(255, 255, 255, a);
    }
}

void BackpackHud::update(float time_elapsed)
{
    size_t i = 0;
    for (auto& special : Game::get().getPlayer().getBackpack())
    {
        special.first.setPosition(placeholders_[i].getPosition());

        if (special.second > 1)
        {
            numbers_[i].setString(std::to_string(special.second));
            numbers_[i].setPosition(placeholders_[i].getPosition() + CONF<sf::Vector2f>("graphics/backpack_number_diff"));
        }

        ++i;
    }
}

void BackpackHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto& placeholder : placeholders_)
    {
        target.draw(placeholder, states);
    }

    size_t i = 0;
    for (auto& special : Game::get().getPlayer().getBackpack())
    {
        target.draw(special.first, states);
        if (special.second > 1)
        {
            target.draw(numbers_[i]);
        }

        ++i;
    }
}


SkillsHud::SkillsHud(const sf::Vector2f& pos)
{
    static std::string texts[] = {"Intelligence: 0", "Heart: 0", "Strength: 0", "Agility: 0"};
    int i = 0;
    for (const auto& line : CONF<j3x::List>("graphics/skills_lines"))
    {
        auto line_pos = j3x::getObj<sf::Vector2f>(line);
        lines_.emplace_back(sf::Color::White, 3.0f);
        lines_.back().setStart(pos + line_pos);
        lines_.back().setEnd(pos + line_pos);
        lines_.back().setThickness(CONF<float>("graphics/lines_thickness"));
        lines_.back().forceSet();

        texts_.emplace_back(texts[i], RM.getFont(), CONF<float>("graphics/skills_text_size"));
        texts_.back().setFillColor(sf::Color::White);
        texts_.back().setPosition(lines_.back().getStart() + CONF<sf::Vector2f>("graphics/skills_text_offset"));

        ++i;
        if (i >= 4)
            break;
    }
}

void SkillsHud::update(float time_elapsed)
{
    for (auto& line : lines_)
    {
        line.update(time_elapsed);
    }
}

void SkillsHud::show(bool no)
{
    if (!no)
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
}

void SkillsHud::setColor(const sf::Color &color)
{
    for (auto& line : lines_)
    {
        line.setColor(color);
    }

    for (auto& text : texts_)
    {
        text.setFillColor(color);
    }
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
}


FullHud::FullHud(const sf::Vector2f& size) :
        show_(false),
        bg_color_(sf::Color(0, 0, 0, 0)),
        bg_(size),
        player_(size / 2.0f + CONF<sf::Vector2f>("graphics/huge_player_offset"),
                CONF<sf::Vector2f>("graphics/huge_player_size"),
                &RM.getTexture("huge_player")),
        backpack_hud_(size / 2.0f + CONF<sf::Vector2f>("graphics/backpack_offset"),
                      CONF<int>("graphics/backpack_placeholders_x"),
                      CONF<int>("graphics/backpack_placeholders_y")),
        skills_hud_(size / 2.0f + CONF<sf::Vector2f>("graphics/skills_offset"))
{
    bg_.setPosition(0.0f, 0.0f);
    bg_.setFillColor(bg_color_);
    player_.setColor(255, 255, 255, bg_color_.a);
    backpack_hud_.setOpacity(bg_color_.a);
}

void FullHud::update(float time_elapsed)
{
    static const auto opacity_factor = CONF<int>("graphics/full_hud_max_opacity") / CONF<float>("graphics/full_hud_show_duration");

    auto delta = static_cast<int>(time_elapsed * opacity_factor);
    if (!show_)
    {
        bg_color_.a = static_cast<sf::Uint8>(std::min(std::max(bg_color_.a - delta, 0), CONF<int>("graphics/full_hud_max_opacity")));
    }
    else
    {
        bg_color_.a = static_cast<sf::Uint8>(std::min(std::max(bg_color_.a + delta, 0), CONF<int>("graphics/full_hud_max_opacity")));
    }
    bg_.setFillColor(bg_color_);
    player_.setColor(255, 255, 255, static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    backpack_hud_.setOpacity(static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity")));
    skills_hud_.setColor({255, 255, 255, static_cast<sf::Uint8>(bg_color_.a * 255.0f / CONF<int>("graphics/full_hud_max_opacity"))});
    skills_hud_.update(time_elapsed);
    backpack_hud_.update(time_elapsed);

    time_elapsed_ = std::max(0.0f, time_elapsed_ - time_elapsed);
}

void FullHud::show(bool show)
{
    if (show != show_)
    {
        time_elapsed_ = CONF<float>("graphics/full_hud_show_duration");
        show_ = show;
    }

    skills_hud_.show(!show_);
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
