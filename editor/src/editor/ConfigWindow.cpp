//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <editor/ConfigWindow.h>
#include <Editor.h>


using namespace editor;

ConfigWindow::ConfigWindow(tgui::Gui *gui, tgui::Theme *theme) :
        ChildWindow(gui, theme, "Edit configuration",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size"),
                    "config_window") {
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Edit configuration:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::TextBox::create();
    text_box_->setRenderer(theme_->getRenderer("TextBox"));
    text_box_->setSize("70%", "65%");

    text_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(text_box_, 1, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));

    grid_->addWidget(button_, 2, 0);

    float padding = CONF<float>("items_padding");
    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
}

void ConfigWindow::setConfigContent(const std::string &category, const std::string &id) {
    child_->setTitle(category + "/" + id);
    text_box_->setText(RM.getConfigContent(category, id));

    button_->connect("pressed", [&](const std::string &category, const std::string &id, tgui::TextBox::Ptr box) {
        Editor::get().saveConfig(category, id, box->getText());
        child_->close();
    }, category, id, text_box_);
}

