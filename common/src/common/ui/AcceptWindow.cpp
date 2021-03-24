//
// Created by jul3x on 23.11.2020.
//

#include <R3E/system/Config.h>

#include <common/ui/AcceptWindow.h>
#include <common/ui/UserInterface.h>

using namespace r3e;

AcceptWindow::AcceptWindow(UserInterface* ui, const std::string& text,
                           const sf::Vector2f& pos, const sf::Vector2f& size) : Window(ui, pos, size)
{
    child_->setTitle("Warning");

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
    no_->connect("pressed", [this, ui](){ ui->closeWindow(this); });

    child_->add(no_);
}

void AcceptWindow::bindFunction(const std::function<void()>& yes)
{
    yes_->connect("pressed", yes);
    yes_->connect("pressed", [this]() { ui_->closeWindow(this);});
}
