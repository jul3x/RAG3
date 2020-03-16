//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/SaveWindow.h>


using namespace editor;

SaveWindow::SaveWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Save map",
                    sf::Vector2f(CFG.getInt("window_width_px") - CFG.getFloat("popup_window_size_x"),
                                 CFG.getInt("window_height_px") - CFG.getFloat("popup_window_size_y")) / 2.0f,
                    {CFG.getFloat("popup_window_size_x"), CFG.getFloat("popup_window_size_y")},
                    "save_window")
{
    grid_ = tgui::Grid::create();
//    grid_->setSize({CFG.getFloat("menu_window_size_x") * 0.75f, CFG.getFloat("menu_window_size_y")});
//    grid_->setPosition({CFG.getFloat("menu_window_size_x") * 0.125f, 0.0f});
//    grid_->setWidgetPadding(0, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
//    grid_->setWidgetPadding(0, 1, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    child_->add(grid_);
}
