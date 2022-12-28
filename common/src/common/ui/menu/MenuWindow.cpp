//
// Created by jul3x on 14.08.21.
//

#include <common/ui/menu/MenuWindow.h>


MenuWindow::MenuWindow(tgui::Gui* gui, tgui::Theme* theme, const std::string& header) :
        gui_(gui), theme_(theme), is_visible_(false),
        background_(CONF<sf::Vector2f>("graphics/menu_window_size")),
        size_(CONF<sf::Vector2f>("graphics/menu_window_size"))
{
    float right_margin = std::max(
        2.0f * CONF<float>("graphics/user_interface_zoom"),
        CONF<int>("graphics/window_width_px") / 2.0f - size_.x / 2.0f - CONF<float>("graphics/menu_bar_width_px") / 2.0f);

    pos_ = {
        CONF<int>("graphics/window_width_px") - right_margin,
        CONF<int>("graphics/window_height_px") / 2.0f
    };

    background_.setFillColor(sf::Color(0, 0, 0, 160));
    background_.setOutlineColor(sf::Color(255, 255, 255, 200));
    background_.setOutlineThickness(2.0f * CONF<float>("graphics/user_interface_zoom"));
    background_.setPosition(pos_);
    background_.setOrigin(size_.x, size_.y / 2.0f);

    header_ = tgui::Label::create();
    header_->setRenderer(theme_->getRenderer("Header"));
    header_->setText(header);
    header_->setTextSize(CONF<float>("graphics/menu_window_header_size"));
    header_->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    header_->setPosition(
        sf::Vector2f{pos_.x - size_.x, pos_.y - size_.y / 2.0f} +
                         CONF<sf::Vector2f>("graphics/menu_window_header_pos"));

    gui_->add(header_);
    header_->setVisible(false);
}

void MenuWindow::doShow(bool show)
{
    is_visible_ = show;
    header_->setVisible(show);
}

bool MenuWindow::isOpen() const
{
    return is_visible_;
}
