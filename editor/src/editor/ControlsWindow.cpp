//
// Created by maciek on 10/08/2021.
//

#include <R3E/utils/Misc.h>

#include <editor/ControlsWindow.h>

#include <Editor.h>


using namespace editor;

ControlsWindow::ControlsWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Editor controls",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size") / 1.3f,
                    "controls_window")
{
    child_->setTitle("Editor controls");

    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "20");
    grid_->setSize("90%", "80%");
    grid_->setAutoSize(true);
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("E - Edit object's parameters\nQ - Clear cursor\n"
                                        "F1 - Paste cursor's current coordinates\nF2 - Paste hovered object's UID\n"
                                        "ESC - Close all windows\nLEFT SHIFT - Align object to the grid\n"
                                        "LEFT SHIFT - Align object to the 0.25 grid size\nLEFT MOUSE BUTTON - Place object\n"
                                        "RIGHT MOUSE BUTTON - Remove object\nMIDDLE MOUSE BUTTON - Move object\n"
                                        "CTRL + MOUSE - Move camera\nMOUSE WHEEL - Zoom");
    label->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(label, 0, 0);

    float padding = CONF<float>("items_padding");
    grid_->setWidgetPadding(0, 0, {padding, padding});
}
