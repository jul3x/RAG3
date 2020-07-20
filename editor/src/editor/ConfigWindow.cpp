//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <editor/ConfigWindow.h>
#include <Editor.h>


using namespace editor;

ConfigWindow::ConfigWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Edit configuration",
                    sf::Vector2f(CFG.get<int>("window_width_px") - CFG.get<float>("popup_window_size_x") * CFG.get<float>("user_interface_zoom"),
                                 CFG.get<int>("window_height_px") - CFG.get<float>("popup_window_size_y") * CFG.get<float>("user_interface_zoom")) / 2.0f,
                    {CFG.get<float>("popup_window_size_x") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("popup_window_size_y") * CFG.get<float>("user_interface_zoom")},
                    "config_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Edit configuration:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::TextBox::create();
    text_box_->setRenderer(theme_->getRenderer("TextBox"));
    text_box_->setSize("70%", "65%");

    text_box_->setTextSize(14 * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(text_box_, 1, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(button_->getTextSize() * CFG.get<float>("user_interface_zoom"));
    button_->setSize(CFG.get<float>("button_size_x") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("button_size_y") * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(button_, 2, 0);

    float padding = CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom");
    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
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

