//
// Created by jul3x on 08.12.2020.
//

#include <common/ui/NoteWindow.h>
#include <common/Framework.h>


NoteWindow::NoteWindow(Framework* framework, UserInterface* ui, const std::string& text,
                       const sf::Vector2f& pos, const sf::Vector2f& size, bool note_info) :
        Window(ui, pos, size),
        framework_(framework)
{
    child_->setTitle(note_info ? "Note\n" : "Information\n");
    child_->setTitleTextSize(CONF<float>("graphics/popup_text_size"));
    child_->setTitleButtons(0);

    auto label = tgui::Label::create();

    if (note_info)
    {
        label->setRenderer(theme_->getRenderer("TooltipHeader"));
        label->setText("You are reading a note:");
        label->setTextSize(CONF<float>("graphics/popup_text_size"));
        label->setPosition("50% - width / 2", "5%");
        child_->add(label);
    }

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("TooltipText"));
    label->setText(note_info ? "\"" + text + "\"" : text);
    label->setTextSize(CONF<float>("graphics/popup_text_size"));
    label->setPosition("50% - width / 2", "15%");
    child_->add(label);

    ok_ = tgui::Button::create();
    ok_->setRenderer(theme_->getRenderer("Button"));
    ok_->setText("OK\n");
    ok_->setTextSize(CONF<float>("graphics/popup_text_size"));
    ok_->setSize(CONF<sf::Vector2f>("graphics/popup_button_size"));
    ok_->setPosition("50% - width/2", "100% - " + std::to_string(CONF<float>("graphics/popup_button_relative_valign")));
    ok_->connect("pressed", [this]() {
        ui_->closeWindow(this);
    });
    child_->add(ok_);
}

