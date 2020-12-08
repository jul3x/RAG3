//
// Created by jul3x on 08.12.2020.
//

#include <ui/Window.h>
#include <Game.h>


Window::Window(tgui::Gui *gui, tgui::Theme *theme, const sf::Vector2f& pos, const sf::Vector2f& size) : gui_(gui), theme_(theme)
{
    child_ = tgui::ChildWindow::create();
    child_->setRenderer(theme_->getRenderer("ChildWindow"));
    child_->setSize(size);
    child_->setPosition(pos - size / 2.0f);
    child_->setTitleTextSize(child_->getTitleTextSize() * CONF<float>("graphics/user_interface_zoom"));
    child_->setResizable(false);
    child_->getRenderer()->setTitleBarHeight(child_->getRenderer()->getTitleBarHeight() * CONF<float>("graphics/user_interface_zoom"));
    child_->connect("closed", [&](){ Game::get().getUI().closeWindow(this); });
    gui_->add(child_);
}

Window::~Window()
{
    gui_->remove(child_);
}