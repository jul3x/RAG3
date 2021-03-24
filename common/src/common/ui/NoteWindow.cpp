//
// Created by jul3x on 08.12.2020.
//

#include <common/ui/NoteWindow.h>
#include <common/Framework.h>


NoteWindow::NoteWindow(Framework* framework, UserInterface* ui, const std::string& text,
                       const sf::Vector2f& pos, const sf::Vector2f& size) :
       Window(ui, pos, size),
       framework_(framework)
{
    child_->setTitle("Note");
    child_->connect("closed", [this]() { framework_->setGameState(Framework::GameState::Normal); ui_->closeWindow(this); });

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("TooltipHeader"));
    label->setText("You are reading a note:");
    label->setTextSize(CONF<float>("graphics/popup_text_size"));
    label->setPosition("50% - width / 2", "5%");
    child_->add(label);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("TooltipText"));
    label->setText("\"" + text + "\"");
    label->setTextSize(CONF<float>("graphics/popup_text_size"));
    label->setPosition("50% - width / 2", "15%");
    child_->add(label);

    ok_ = tgui::Button::create();
    ok_->setRenderer(theme_->getRenderer("Button"));
    ok_->setText("OK");
    ok_->setTextSize(CONF<float>("graphics/popup_text_size"));
    ok_->setSize(CONF<sf::Vector2f>("graphics/popup_button_size"));
    ok_->setPosition("50% - width/2", "100% - " + std::to_string(CONF<float>("graphics/popup_button_relative_valign")));
    ok_->connect("pressed", [this]() { framework_->setGameState(Framework::GameState::Normal); ui_->closeWindow(this); });
    child_->add(ok_);
}

