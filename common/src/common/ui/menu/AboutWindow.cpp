//
// Created by jul3x on 14.08.21.
//

#include <R3E/system/Config.h>

#include <common/ui/menu/AboutWindow.h>


AboutWindow::AboutWindow(tgui::Gui* gui, tgui::Theme* theme) :
        MenuWindow(gui, theme, "> About")
{
    text_ = tgui::Label::create();
    text_->setRenderer(theme->getRenderer("Text"));
    auto wrapped_text = utils::wrapText(std::move(CONF<std::string>("about_text")), CONF<int>("about_text_max_chars_per_line"));

    text_->setText(wrapped_text);

    text_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    text_->setPosition(sf::Vector2f{pos_.x - size_.x, pos_.y - size_.y / 2.0f + CONF<float>("graphics/menu_window_header_size") * 2.0f} +
                       CONF<sf::Vector2f>("graphics/menu_window_header_pos"));

    text_->setVisible(false);
    gui->add(text_);
}

void AboutWindow::doShow(bool show)
{
    MenuWindow::doShow(show);
    text_->setVisible(show);
}
