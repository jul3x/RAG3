//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/SaveWindow.h>
#include <Editor.h>


using namespace editor;

SaveWindow::SaveWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Save map",
                    sf::Vector2f(CFG.getInt("window_width_px") - CFG.getFloat("popup_window_size_x"),
                                 CFG.getInt("window_height_px") - CFG.getFloat("popup_window_size_y")) / 2.0f,
                    {CFG.getFloat("popup_window_size_x"), CFG.getFloat("popup_window_size_y")},
                    "save_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Enter map name:");
    label->setTextSize(14);

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::EditBox::create();
    text_box_->setRenderer(theme_->getRenderer("EditBox"));
    text_box_->setSize("70%", 20);
    grid_->addWidget(text_box_, 1, 0);

    auto button = tgui::Button::create();
    button->setRenderer(theme_->getRenderer("Button"));
    button->setText("Save");
    button->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));
    button->connect("pressed", [&](){ Editor::get().saveMap(text_box_->getText()); child_->close(); });

    grid_->addWidget(button, 2, 0);

    grid_->setWidgetPadding(0, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    grid_->setWidgetPadding(1, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    grid_->setWidgetPadding(2, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
}


void SaveWindow::refreshMapName(const std::string& map_name)
{
    text_box_->setText(map_name);
}
