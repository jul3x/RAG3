//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/SaveWindow.h>
#include <Editor.h>


using namespace editor;

SaveWindow::SaveWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Save map",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                     CONF<sf::Vector2f>("popup_window_size"),
                    "save_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Enter map name:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::EditBox::create();
    text_box_->setRenderer(theme_->getRenderer("EditBox"));
    text_box_->setSize("70%", CONF<float>("text_box_height"));
    text_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(text_box_, 1, 0);

    auto button = tgui::Button::create();
    button->setRenderer(theme_->getRenderer("Button"));
    button->setText("Save");
    button->setTextSize(CONF<float>("text_box_height"));
    button->setSize(CONF<sf::Vector2f>("button_size"));
    button->connect("pressed", [&](){ Editor::get().saveMap(text_box_->getText()); child_->close(); });

    grid_->addWidget(button, 2, 0);

    float padding = CONF<float>("items_padding");
    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
}


void SaveWindow::refreshMapName(const std::string& map_name)
{
    text_box_->setText(map_name);
}
