//
// Created by jul3x on 23.11.2020.
//

#include <ui/AcceptWindow.h>
#include <Game.h>


AcceptWindow::AcceptWindow(tgui::Gui *gui, tgui::Theme *theme, const std::string& text,
                           const sf::Vector2f& pos, const sf::Vector2f& size) : gui_(gui), theme_(theme)
{
    child_ = tgui::ChildWindow::create();
    child_->setRenderer(theme_->getRenderer("ChildWindow"));
    child_->setSize(size);
    child_->setPosition(pos - size / 2.0f);
    child_->setTitle("Warning");
    child_->setResizable(false);
    child_->setTitleTextSize(child_->getTitleTextSize() * CONF<float>("graphics/user_interface_zoom"));
    child_->connect("closed", [&](){ Game::get().getUI().closeAcceptWindow(this); });
    gui_->add(child_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText(text);
    label->setTextSize(CONF<float>("graphics/popup_text_size"));
    label->setPosition("50% - width / 2", "20%");
    child_->add(label);

    yes_ = tgui::Button::create();
    yes_->setRenderer(theme_->getRenderer("Button"));
    yes_->setText("Yes");
    yes_->setTextSize(CONF<float>("graphics/popup_text_size"));
    yes_->setSize(CONF<sf::Vector2f>("graphics/popup_button_size"));
    yes_->setPosition("33% - width/2", "100% - " + std::to_string(CONF<float>("graphics/popup_button_relative_valign")));

    child_->add(yes_);

    no_ = tgui::Button::create();
    no_->setRenderer(theme_->getRenderer("Button"));
    no_->setText("No");
    no_->setTextSize(CONF<float>("graphics/popup_text_size"));
    no_->setSize(CONF<sf::Vector2f>("graphics/popup_button_size"));
    no_->setPosition("66% - width/2", "100% - " + std::to_string(CONF<float>("graphics/popup_button_relative_valign")));
    no_->connect("pressed", [&](){ Game::get().getUI().closeAcceptWindow(this); });

    child_->add(no_);
}

void AcceptWindow::bindFunction(const std::function<void()>& yes)
{
    yes_->connect("pressed", yes);
    yes_->connect("pressed", [&]() {Game::get().getUI().closeAcceptWindow(this);});
}

AcceptWindow::~AcceptWindow()
{
    gui_->remove(child_);
}