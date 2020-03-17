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
    grid_->setSize({CFG.getFloat("menu_window_size_x") * 0.75f, CFG.getFloat("menu_window_size_y") * 0.9f});
    grid_->setPosition({CFG.getFloat("menu_window_size_x") * 0.125f, CFG.getFloat("menu_window_size_y") * 0.05f});
    child_->add(grid_);

    auto button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Load map");
    button->connect("Clicked", [this]() { this->ui_->resetMapList(); this->gui_->get("load_window")->setVisible(true); });
    grid_->addWidget(button, 0, 0);

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Save map");
    grid_->addWidget(button, 0, 1);
    button->connect("Clicked", [this]() { this->ui_->resetMapList(); this->gui_->get("save_window")->setVisible(true); });

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Edit config");
    grid_->addWidget(button, 1, 0);
    button->connect("Clicked", [this]() { this->ui_->openConfigWindow("config_dir", "config"); });

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Edit graphics");
    grid_->addWidget(button, 1, 1);
    button->connect("Clicked", [this]() { this->ui_->openConfigWindow("config_dir", "graphics"); });

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Edit characters");
    grid_->addWidget(button, 2, 0);
    button->connect("Clicked", [this]() { this->ui_->openConfigWindow("config_dir", "characters"); });

    button = tgui::Button::create();
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->setText("Edit sound");
    grid_->addWidget(button, 2, 1);
    button->connect("Clicked", [this]() { this->ui_->openConfigWindow("config_dir", "sound"); });
}
