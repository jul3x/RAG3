//
// Created by jul3x on 14.08.21.
//

#include <common/ui/menu/MenuWindow.h>


MenuWindow::MenuWindow(tgui::Gui* gui, tgui::Theme* theme, const std::string& header) :
        gui_(gui), theme_(theme), is_visible_(false),
        background_(CONF<sf::Vector2f>("graphics/menu_window_size"))
{
    auto pos = CONF<sf::Vector2f>("graphics/menu_window_pos") +
               sf::Vector2f((CONF<float>("graphics/menu_bar_width_px") + CONF<int>("graphics/window_width_px")) / 2.0f,
                            0.0f);
    background_.setFillColor(sf::Color(0, 0, 0, 160));
    background_.setOutlineColor(sf::Color(255, 255, 255, 200));
    background_.setOutlineThickness(2.0f * CONF<float>("graphics/user_interface_zoom"));
    background_.setPosition(pos);
    background_.setOrigin(CONF<sf::Vector2f>("graphics/menu_window_size").x / 2.0f, 0.0f);

    header_ = tgui::Label::create();
    header_->setRenderer(theme_->getRenderer("Header"));
    header_->setText(header);
    header_->setInheritedFont(RM.getFont("default"));
    header_->setTextSize(CONF<float>("graphics/menu_window_header_size"));
    header_->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    header_->setPosition(sf::Vector2f{pos.x - CONF<sf::Vector2f>("graphics/menu_window_size").x / 2.0f, pos.y} +
                         CONF<sf::Vector2f>("graphics/menu_window_header_pos"));

    gui_->add(header_);

    doShow(false);
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
