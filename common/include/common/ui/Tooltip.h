//
// Created by jul3x on 12.11.2020.
//

#ifndef RAG3_GAME_INCLUDE_UI_TOOLTIP_H
#define RAG3_GAME_INCLUDE_UI_TOOLTIP_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>

class Framework;

using namespace r3e;

class Tooltip {
public:
    explicit Tooltip(Framework* framework, tgui::Theme* theme, const sf::Vector2f& position, const sf::Vector2f& size);

    void bindText(const std::string& header, const std::string& text);

    void bindFunction(const std::function<void()>& func);

    void bindGui(tgui::Gui* gui);

    void show(bool hide = false);

    void setActive(bool active);

    [[nodiscard]] bool isActive() const;

    [[nodiscard]] tgui::Panel::Ptr getTooltip();

private:
    tgui::Theme* theme_;
    tgui::Button::Ptr button_;
    tgui::Label::Ptr tooltip_, tooltip_header_;
    tgui::Panel::Ptr layout_;

    bool active_{false};

};

#endif //RAG3_GAME_INCLUDE_UI_TOOLTIP_H
