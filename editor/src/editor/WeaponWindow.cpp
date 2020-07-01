//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/WeaponWindow.h>
#include <Editor.h>


using namespace editor;

WeaponWindow::WeaponWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Special editor",
                    sf::Vector2f(CFG.get<int>("window_width_px") - CFG.get<float>("popup_window_size_x") * CFG.get<float>("user_interface_zoom"),
                                 CFG.get<int>("window_height_px") - CFG.get<float>("popup_window_size_y") * CFG.get<float>("user_interface_zoom")) / 2.0f,
                    {CFG.get<float>("popup_window_size_x") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("popup_window_size_y") * CFG.get<float>("user_interface_zoom")},
                    "weapon_window"),
        weapon_(nullptr)
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "40% - height/2");
    grid_->setSize("90%", "50%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Identifier:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 0, 0);

    id_box_ = tgui::EditBox::create();
    id_box_->setRenderer(theme_->getRenderer("EditBox"));
    id_box_->setReadOnly(true);
    id_box_->setSize("35%", 20 * CFG.get<float>("user_interface_zoom"));
    id_box_->setTextSize(14 * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(id_box_, 1, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Usage function:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 2, 0);

    fun_box_ = tgui::TextBox::create();
    fun_box_->setRenderer(theme_->getRenderer("TextBox"));
    fun_box_->setSize("35%", "45%");
    fun_box_->setTextSize(14 * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(fun_box_, 3, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Function data:");
    label->setTextSize(14 * CFG.get<float>("user_interface_zoom"));

    grid_->addWidget(label, 2, 1);

    data_box_ = tgui::TextBox::create();
    data_box_->setRenderer(theme_->getRenderer("TextBox"));
    data_box_->setSize("35%", "45%");
    data_box_->setTextSize(14 * CFG.get<float>("user_interface_zoom"));
    grid_->addWidget(data_box_, 3, 1);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(button_->getTextSize() * CFG.get<float>("user_interface_zoom"));
    button_->setSize(CFG.get<float>("button_size_x") * CFG.get<float>("user_interface_zoom"), CFG.get<float>("button_size_y") * CFG.get<float>("user_interface_zoom"));
    button_->setPosition("50% - width/2", "100% - " + std::to_string(60 * CFG.get<float>("user_interface_zoom")));

    child_->add(button_);

    float padding = CFG.get<float>("items_padding") * CFG.get<float>("user_interface_zoom");
    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
    grid_->setWidgetPadding(3, 0, {padding, padding});
    grid_->setWidgetPadding(2, 1, {padding, padding});
    grid_->setWidgetPadding(3, 1, {padding, padding});
}

void WeaponWindow::setObjectContent(const std::string& category, PlacedWeapon* obj)
{
    weapon_ = obj;
    child_->setTitle(category + "/" + weapon_->getId());
    id_box_->setText(std::to_string(weapon_->getUniqueId()));
    fun_box_->setText(weapon_->getUsageStr());
    data_box_->setText(std::to_string(weapon_->getData()));

    button_->connect("pressed", [this]() {
        try
        {
            std::string data_str = this->data_box_->getText();
            this->weapon_->setUsageStr(this->fun_box_->getText());
            this->weapon_->setData(std::stof(data_str));
            child_->close();
        }
        catch (const std::exception& e)
        {
            Editor::get().spawnError("Wrong data format! Needed floating point number.");
        }
    });
}

