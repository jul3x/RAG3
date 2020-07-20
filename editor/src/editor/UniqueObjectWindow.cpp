//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/UniqueObjectWindow.h>


using namespace editor;

UniqueObjectWindow::UniqueObjectWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Unique identifier of object",
                    sf::Vector2f(CFG.get<int>("window_width_px") - CFG.get<float>("popup_window_size_x") * CFG.get<float>("user_interface_zoom"),
                                 CFG.get<int>("window_height_px") - CFG.get<float>("popup_window_size_y") * CFG.get<float>("user_interface_zoom")) / 2.0f,
                    {CFG.get<float>("unique_object_window_size_x") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("unique_object_window_size_y") * CFG.get<float>("user_interface_zoom")},
                    "unique_object_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Identifier:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::EditBox::create();
    text_box_->setRenderer(theme_->getRenderer("EditBox"));
    text_box_->setReadOnly(true);
    text_box_->setSize("70%", 20 * CFG.get<float>("user_interface_zoom"));
    text_box_->setTextSize(14 * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(text_box_, 1, 0);

    grid_->setWidgetPadding(0, 0, {CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom")});
    grid_->setWidgetPadding(1, 0, {CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom")});
}

void UniqueObjectWindow::setObjectContent(const std::string& category, const std::string& name, int id)
{
    child_->setTitle(category + "/" + name);
    text_box_->setText(std::to_string(id));
}

