//
// Created by jul3x on 30.03.2021.
//

//
// Created by jul3x on 12.11.2020.
//

#include <common/Framework.h>
#include <common/ui/Tooltip.h>


Tooltip::Tooltip(Framework* framework, tgui::Theme* theme, const sf::Vector2f& position) : theme_(theme)
{
    button_ = tgui::Button::create();
    button_->setRenderer(theme->getRenderer("TooltipButton"));
    button_->setPosition(position);
    button_->setSize(CONF<sf::Vector2f>("graphics/backpack_placeholder_size"));

    layout_ = tgui::Panel::create();
    layout_->setRenderer(theme->getRenderer("Tooltip"));
    tooltip_ = tgui::Label::create();
    tooltip_->setRenderer(theme->getRenderer("TooltipText"));
    tooltip_->setTextSize(CONF<float>("graphics/tooltip_text_size"));
    tooltip_->setPosition("0", CONF<float>("graphics/tooltip_text_size") * 1.5f);
    layout_->add(tooltip_);

    tooltip_header_ = tgui::Label::create();
    tooltip_header_->setRenderer(theme->getRenderer("TooltipHeader"));
    tooltip_header_->setTextSize(CONF<float>("graphics/tooltip_text_size"));
    layout_->add(tooltip_header_);

    layout_->setPosition(-tgui::bindWidth(tooltip_) - 20.0f, 0);
    layout_->setSize(tgui::bindWidth(tooltip_) + 20.0f,
                     tgui::bindHeight(tooltip_) + 1.5f * tgui::bindHeight(tooltip_header_) + 20.0f);
    button_->setToolTip(layout_);

    button_->connect("mouseentered", [framework]() {
        framework->spawnSound(RM.getSound("ui_hover"), framework->getPlayer()->getPosition()); });
}

void Tooltip::bindText(const std::string& header, const std::string& text)
{
    if (header != tooltip_header_->getText())
        layout_->setPosition(-tgui::bindWidth(tooltip_) - 20.0f, 0);

    tooltip_header_->setText(header);
    tooltip_->setText(text);

    if (header.empty() && text.empty())
        button_->setToolTip(nullptr);
    else
        button_->setToolTip(layout_);
}

void Tooltip::bindFunction(const std::function<void()>& func)
{
    button_->connect("pressed", func);
}

void Tooltip::bindGui(tgui::Gui* gui)
{
    gui->add(button_);
}

void Tooltip::show(bool hide)
{
    button_->setVisible(!hide);
}

void Tooltip::setActive(bool active)
{
    active_ = active;
    button_->setRenderer(theme_->getRenderer(active ? "TooltipButtonActive" : "TooltipButton"));
}

bool Tooltip::isActive() const
{
    return active_;
}
