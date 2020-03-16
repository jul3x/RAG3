//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/MenuWindow.h>
#include <editor/UserInterface.h>


using namespace editor;

MenuWindow::MenuWindow(UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme) :
        ui_(ui),
        ChildWindow(gui, theme, "Menu",
                    {CFG.getFloat("menu_window_x"), CFG.getFloat("menu_window_y")},
                    {CFG.getFloat("menu_window_size_x"), CFG.getFloat("menu_window_size_y")},
                    "menu_window")
{
    grid_ = tgui::Grid::create();
    grid_->setSize({CFG.getFloat("menu_window_size_x") * 0.75f, CFG.getFloat("menu_window_size_y")});
    grid_->setPosition({CFG.getFloat("menu_window_size_x") * 0.125f, 0.0f});
    grid_->setWidgetPadding(0, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    grid_->setWidgetPadding(0, 1, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    child_->add(grid_);

    auto button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Load map");
    button->connect("Clicked", [&]() { ui_->resetMapList(); gui_->get("load_window")->setVisible(true); });
    grid_->addWidget(button, 0, 0);

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Save map");
    grid_->addWidget(button, 0, 1);
    button->connect("Clicked", [&]() { ui_->resetMapList(); gui_->get("save_window")->setVisible(true); });
}
