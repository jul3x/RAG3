//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/ResourceManager.h>
#include <editor/ConfigWindow.h>
#include <Editor.h>


using namespace editor;

ConfigWindow::ConfigWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Edit configuration",
                    sf::Vector2f(CFG.getInt("window_width_px") - CFG.getFloat("popup_window_size_x"),
                                 CFG.getInt("window_height_px") - CFG.getFloat("popup_window_size_y")) / 2.0f,
                    {CFG.getFloat("popup_window_size_x"), CFG.getFloat("popup_window_size_y")},
                    "config_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Edit configuration:");
    label->setTextSize(14);

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::TextBox::create();
    text_box_->setRenderer(theme_->getRenderer("TextBox"));
    text_box_->setSize("70%", "65%");

    text_box_->setTextSize(14);
    grid_->addWidget(text_box_, 1, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setSize(CFG.getFloat("button_size_x"), CFG.getFloat("button_size_y"));

    grid_->addWidget(button_, 2, 0);

    grid_->setWidgetPadding(0, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    grid_->setWidgetPadding(1, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
    grid_->setWidgetPadding(2, 0, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
}

void ConfigWindow::setConfigContent(const std::string& category, const std::string& id)
{
    child_->setTitle(category + "/" + id);
    text_box_->setText(RM.getConfigContent(category, id));

    button_->connect("pressed", [&](const std::string& category, const std::string& id, tgui::TextBox::Ptr box) {
        Editor::get().saveConfig(category, id, box->getText());
        child_->close();
    }, category, id, text_box_);
}

