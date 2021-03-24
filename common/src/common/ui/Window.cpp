//
// Created by jul3x on 08.12.2020.
//

#include <common/ui/Window.h>
#include <common/ui/UserInterface.h>


Window::Window(UserInterface* ui, const sf::Vector2f& pos, const sf::Vector2f& size) :
        ui_(ui), gui_(ui->getGui()), theme_(ui->getTheme())
{
    child_ = tgui::ChildWindow::create();
    child_->setRenderer(theme_->getRenderer("ChildWindow"));
    child_->setSize(size);
    child_->setPosition(pos - size / 2.0f);
    child_->setTitleTextSize(child_->getTitleTextSize() * CONF<float>("graphics/user_interface_zoom"));
    child_->setResizable(false);
    child_->getRenderer()->setTitleBarHeight(child_->getRenderer()->getTitleBarHeight() * CONF<float>("graphics/user_interface_zoom"));
    child_->connect("closed", [this](){ ui_->closeWindow(this); });
    gui_->add(child_);
}

Window::~Window()
{
    gui_->remove(child_);
}