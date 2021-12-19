//
// Created by jul3x on 17.10.21.
//

#include <R3E/system/Config.h>

#include <common/Framework.h>

#include <common/ui/menu/PickServerWindow.h>


PickServerWindow::PickServerWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework) :
        MenuWindow(gui, theme, "> Pick server"),
        framework_(framework)
{
    auto pos = CONF<sf::Vector2f>("graphics/menu_window_pos") +
               sf::Vector2f((CONF<float>("graphics/menu_bar_width_px") + CONF<int>("graphics/window_width_px")) / 2.0f,
                            CONF<float>("graphics/menu_window_header_size") * 2.0f);

    text_ = tgui::Label::create("Enter IP address or network name:");
    text_->setRenderer(theme->getRenderer("Text"));
    text_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    text_->setPosition(sf::Vector2f{pos.x - CONF<sf::Vector2f>("graphics/menu_window_size").x / 2.0f, pos.y} +
                       CONF<sf::Vector2f>("graphics/menu_window_header_pos"));
    gui->add(text_);

    editbox_ = tgui::EditBox::create();
    editbox_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    editbox_->setSize(CONF<sf::Vector2f>("graphics/menu_edit_box_size"));
    editbox_->setRenderer(theme->getRenderer("EditBoxGame"));
    editbox_->setPosition(pos - CONF<sf::Vector2f>("graphics/menu_edit_box_size") / 2.0f +
                          sf::Vector2f(0.0f, CONF<sf::Vector2f>("graphics/menu_window_size").y * 0.3f));
    editbox_->setMaximumCharacters(15);
    gui->add(editbox_);

    button_ = tgui::Button::create("Connect");
    button_->setRenderer(theme->getRenderer("ButtonGame"));
    button_->getRenderer()->setBorders(tgui::Borders(2, 2, 2, 2));
    button_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    button_->setFocusable(true);
    button_->setSize(CONF<sf::Vector2f>("graphics/menu_connect_button_size"));
    button_->setPosition(pos - CONF<sf::Vector2f>("graphics/menu_connect_button_size") / 2.0f +
                         sf::Vector2f(0.0f, CONF<sf::Vector2f>("graphics/menu_window_size").y * 0.5f));
    button_->connect("mousereleased", [this]() {
        framework_->startGame(editbox_->getText());
    });
    gui->add(button_);

    doShow(false);
}

void PickServerWindow::doShow(bool show)
{
    MenuWindow::doShow(show);
    text_->setVisible(show);
    editbox_->setVisible(show);
    button_->setVisible(show);
}
