//
// Created by jul3x on 08.04.20.
//

#include <functional>

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/hud/SkillsHud.h>
#include <common/ui/UserInterface.h>
#include <common/Framework.h>


SkillsHud::SkillsHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos) :
        points_text_("", RM.getFont(), CONF<float>("graphics/skills_text_size")),
        framework_(framework)
{
    int i = 0;
    for (const auto& line : CONF<j3x::List>("graphics/skills_lines"))
    {
        const auto& skill_name = framework_->getPlayer()->getSkillName(Player::SKILLS[i]);
        auto line_pos = j3x::getObj<sf::Vector2f>(line);
        lines_.emplace_back(sf::Color::White, 3.0f);
        lines_.back().setStart(pos + line_pos);
        lines_.back().setEnd(pos + line_pos);
        lines_.back().setThickness(CONF<float>("graphics/lines_thickness"));
        lines_.back().forceSet();

        texts_.emplace_back(skill_name + ": 0", RM.getFont(), CONF<float>("graphics/skills_text_size"));
        texts_.back().setFillColor(sf::Color::White);
        texts_.back().setPosition(lines_.back().getStart() + CONF<sf::Vector2f>("graphics/skills_text_offset"));

        auto button_pos = texts_.back().getPosition() + sf::Vector2f(texts_.back().getLocalBounds().width, 0)
                          + CONF<sf::Vector2f>("graphics/skills_button_offset");
        buttons_.emplace_back(tgui::Button::create("i\n"));
        buttons_.back()->setRenderer(ui->getTheme()->getRenderer("AddSkillButton"));
        buttons_.back()->setPosition(button_pos);
        buttons_.back()->setSize(CONF<sf::Vector2f>("graphics/skills_button_size"));
        buttons_.back()->setTextSize(CONF<float>("graphics/skills_button_text_size"));
        buttons_.back()->setVisible(false);
        buttons_.back()->connect("pressed", [this, ui](Player::Skills skill) {
            framework_->spawnSound(RM.getSound("ui_click"));
            if (framework_->getPlayer()->isAlive() && !framework_->getPlayer()->addSkill(skill))
            {
                for (auto& button : buttons_)
                    button->setText("i\n");
            }
        }, Player::SKILLS[i]);
        tooltips_.emplace_back(framework_, ui->getTheme(), button_pos,
                               CONF<sf::Vector2f>("graphics/skills_button_size"));
        tooltips_.back().bindText(skill_name, texts_tooltips_[i]);
        buttons_.back()->setToolTip(tooltips_.back().getTooltip());
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
        const auto& skill_name = framework_->getPlayer()->getSkillName(Player::SKILLS[i]);
        text.setString(skill_name + ": " + std::to_string(framework_->getPlayer()->getSkill(Player::SKILLS[i])));
        ++i;
    }

    points_text_.setString("Points: " + std::to_string(framework_->getPlayer()->getSkillPoints()));
}

void SkillsHud::show(bool show)
{
    if (show)
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
        if (framework_->getPlayer()->isAlive() && framework_->getPlayer()->getSkillPoints() > 0)
        {
            button->setText("+\n");
        }
        else
        {
            button->setText("i\n");
        }

        if (!show && button->isVisible())
        {
            button->hideWithEffect(tgui::ShowAnimationType::Fade,
                                   sf::seconds(CONF<float>("graphics/full_hud_show_duration") / 2.0f));
        }
        else if (show && !button->isVisible())
        {
            button->showWithEffect(tgui::ShowAnimationType::Fade,
                                   sf::seconds(CONF<float>("graphics/full_hud_show_duration")));
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
