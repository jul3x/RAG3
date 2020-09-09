//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/LoadWindow.h>
#include <Editor.h>


using namespace editor;

LoadWindow::LoadWindow(tgui::Gui *gui, tgui::Theme *theme) :
        ChildWindow(gui, theme, "Load map",
                    (sf::Vector2f(CFG.get<int>("window_width_px"), CFG.get<int>("window_height_px")) -
                     CFG.get<float>("user_interface_zoom") * CFG.get<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CFG.get<float>("user_interface_zoom") * CFG.get<sf::Vector2f>("popup_window_size"),
                    "load_window") {
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Select desired map:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 0, 0);

    list_box_ = tgui::ListBox::create();
    list_box_->setRenderer(theme_->getRenderer("ListBox"));
    list_box_->setSize("70%", "65%");
    list_box_->setItemHeight(24 * CFG.get<float>("user_interface_zoom"));
    list_box_->setTextSize(list_box_->getTextSize() * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(list_box_, 1, 0);

    auto button = tgui::Button::create();
    button->setRenderer(theme_->getRenderer("Button"));
    button->setText("Load");
    button->setTextSize(button->getTextSize() * CFG.get<float>("user_interface_zoom"));
    button->setSize(CFG.get<sf::Vector2f>("button_size") * CFG.get<float>("user_interface_zoom"));
    button->connect("pressed", [&]() {
        Editor::get().loadMap(list_box_->getSelectedItem());
        child_->close();
    });
    list_box_->connect("DoubleClicked", [&]() {
        Editor::get().loadMap(list_box_->getSelectedItem());
        child_->close();
    });

    grid_->addWidget(button, 2, 0);

    float padding = CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom");
    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
}

void LoadWindow::refreshMapList(const std::vector<std::string> &map_list) {
    list_box_->removeAllItems();

    for (const auto &map : map_list) {
        list_box_->addItem(map);
    }
}

