//
// Created by jul3x on 26.05.2021.
//

#include <common/ResourceManager.h>
#include <common/ui/TextButton.h>
#include <common/Framework.h>


TextButton::TextButton(Framework* framework, const std::string& text,
                       const sf::Vector2f& pos, float size, float show_duration) :
       framework_(framework),
       show_duration_(show_duration)
{
    button_ = tgui::Button::create();
    button_->setRenderer(framework_->getUI()->getTheme()->getRenderer("MenuButton"));
    button_->setText(text);
    button_->setInheritedFont(RM.getFont("default"));
    button_->setTextSize(size);
    button_->setPosition({pos.x - button_->getFullSize().x / 2.0f, pos.y - button_->getFullSize().y / 2.0f});
    button_->setVisible(false);
    button_->connect("mouseentered", [this]() {
        framework_->spawnSound(RM.getSound("ui_hover"), framework_->getPlayer()->getPosition()); });
    button_->connect("pressed", [this]() {
        framework_->spawnSound(RM.getSound("ui_upgrade"), framework_->getPlayer()->getPosition(), true);
    });
}

void TextButton::bindFunction(const std::function<void()>& func)
{
    button_->connect("pressed", func);
    framework_->getUI()->getGui()->add(button_);
}

void TextButton::show(bool hide)
{
    if (!hide)
        button_->showWithEffect(tgui::ShowAnimationType::Fade, sf::seconds(show_duration_));
    else
        button_->hideWithEffect(tgui::ShowAnimationType::Fade, sf::seconds(show_duration_ / 2.0f));
}
