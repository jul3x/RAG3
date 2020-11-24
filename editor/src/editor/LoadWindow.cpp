//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/LoadWindow.h>
#include <Editor.h>


using namespace editor;

LoadWindow::LoadWindow(tgui::Gui *gui, tgui::Theme *theme) :
        ChildWindow(gui, theme, "Load map",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_heigth_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size"),
                    "load_window") {
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Select desired map:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    list_box_ = tgui::ListBox::create();
    list_box_->setRenderer(theme_->getRenderer("ListBox"));
    list_box_->setSize("70%", "65%");
    list_box_->setItemHeight(CONF<float>("load_items_height"));
    list_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(list_box_, 1, 0);

    auto button = tgui::Button::create();
    button->setRenderer(theme_->getRenderer("Button"));
    button->setText("Load");
    button->setTextSize(CONF<float>("label_text_size"));
    button->setSize(CONF<sf::Vector2f>("button_size"));
    button->connect("pressed", [&]() {
        Editor::get().loadMap(list_box_->getSelectedItem());
        child_->close();
    });
    list_box_->connect("DoubleClicked", [&]() {
        Editor::get().loadMap(list_box_->getSelectedItem());
        child_->close();
    });

    grid_->addWidget(button, 2, 0);

    float padding = CONF<float>("items_padding");
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

