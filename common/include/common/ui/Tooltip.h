//
// Created by jul3x on 12.11.2020.
//

#ifndef RAG3_GAME_INCLUDE_UI_TOOLTIP_H
#define RAG3_GAME_INCLUDE_UI_TOOLTIP_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>


using namespace r3e;

class Tooltip {
public:
    explicit Tooltip(tgui::Theme* theme, const sf::Vector2f& position) : theme_(theme)
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

        layout_->setPosition(- tgui::bindWidth(tooltip_) - 20.0f, 0);
        layout_->setSize(tgui::bindWidth(tooltip_) + 20.0f, tgui::bindHeight(tooltip_) + 1.5f * tgui::bindHeight(tooltip_header_) + 20.0f);
        button_->setToolTip(layout_);
    }

    void bindText(const std::string& header, const std::string& text)
    {
        if (header != tooltip_header_->getText())
            layout_->setPosition(- tgui::bindWidth(tooltip_) - 20.0f, 0);

        tooltip_header_->setText(header);
        tooltip_->setText(text);

        if (header.empty() && text.empty())
            button_->setToolTip(nullptr);
        else
            button_->setToolTip(layout_);
    }

    void bindFunction(const std::function<void()>& func)
    {
        button_->connect("pressed", func);
    }

    void bindGui(tgui::Gui* gui)
    {
        gui->add(button_);
    }

    void show(bool hide = false)
    {
        button_->setVisible(!hide);
    }

    void setActive(bool active)
    {
        active_ = active;
        button_->setRenderer(theme_->getRenderer(active ? "TooltipButtonActive" : "TooltipButton"));
    }

    [[nodiscard]] bool isActive() const
    {
        return active_;
    }

private:
    tgui::Theme* theme_;
    tgui::Button::Ptr button_;
    tgui::Label::Ptr tooltip_, tooltip_header_;
    tgui::Panel::Ptr layout_;

    bool active_{false};

};

#endif //RAG3_GAME_INCLUDE_UI_TOOLTIP_H
