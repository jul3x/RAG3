//
// Created by jul3x on 16.03.20.
//

#include <iostream>

#include <R3E/system/Config.h>

#include <editor/ChildWindow.h>


using namespace editor;

ChildWindow::ChildWindow(tgui::Gui* gui,
                         tgui::Theme* theme,
                         std::string title,
                         const sf::Vector2f& pos,
                         const sf::Vector2f& size,
                         std::string id) :
        gui_(gui), theme_(theme), title_(std::move(title)), pos_(pos)
{
    child_ = tgui::ChildWindow::create();
    child_->setRenderer(theme_->getRenderer("ChildWindow"));
    child_->setSize(size);
    child_->setPosition(pos_);
    child_->setTitle(title_);
    child_->setResizable(true);
    child_->setMinimumSize(size);
    child_->setTitleTextSize(child_->getTitleTextSize() * CONF<float>("user_interface_zoom"));
    child_->connect("closed", [](tgui::ChildWindow::Ptr c) { c->setVisible(false); });
    gui_->add(child_, id);
}
