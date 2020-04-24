//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/SpecialObjectWindow.h>


using namespace editor;

SpecialObjectWindow::SpecialObjectWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Special editor",
                    sf::Vector2f(CFG.get<int>("window_width_px") - CFG.get<float>("popup_window_size_x"),
                                 CFG.get<int>("window_height_px") - CFG.get<float>("popup_window_size_y")) / 2.0f,
                    {CFG.get<float>("popup_window_size_x"), CFG.get<float>("popup_window_size_y")},
                    "special_object_window"),
        functional_(nullptr)
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "40% - height/2");
    grid_->setSize("90%", "50%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Identifier:");
    label->setTextSize(14);

    grid_->addWidget(label, 0, 0);

    id_box_ = tgui::EditBox::create();
    id_box_->setRenderer(theme_->getRenderer("EditBox"));
    id_box_->setReadOnly(true);
    id_box_->setSize("35%", 20);
    id_box_->setTextSize(14);
    grid_->addWidget(id_box_, 1, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Activate:");
    label->setTextSize(14);

    grid_->addWidget(label, 0, 1);

    act_box_ = tgui::EditBox::create();
    act_box_->setRenderer(theme_->getRenderer("EditBox"));
    act_box_->setSize("35%", 20);
    act_box_->setTextSize(14);
    grid_->addWidget(act_box_, 1, 1);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Function:");
    label->setTextSize(14);

    grid_->addWidget(label, 2, 0);

    fun_box_ = tgui::TextBox::create();
    fun_box_->setRenderer(theme_->getRenderer("TextBox"));
    fun_box_->setSize("35%", "45%");
    fun_box_->setTextSize(14);
    grid_->addWidget(fun_box_, 3, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Function data:");
    label->setTextSize(14);

    grid_->addWidget(label, 2, 1);

    data_box_ = tgui::TextBox::create();
    data_box_->setRenderer(theme_->getRenderer("TextBox"));
    data_box_->setSize("35%", "45%");
    data_box_->setTextSize(14);
    grid_->addWidget(data_box_, 3, 1);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setSize(CFG.get<float>("button_size_x"), CFG.get<float>("button_size_y"));
    button_->setPosition("50% - width/2", "100% - 60");

    child_->add(button_);

    grid_->setWidgetPadding(0, 0, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(1, 0, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(0, 1, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(1, 1, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(2, 0, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(3, 0, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(2, 1, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
    grid_->setWidgetPadding(3, 1, {CFG.get<float>("items_padding"), CFG.get<float>("items_padding")});
}

void SpecialObjectWindow::setObjectContent(const std::string& category, Functional* obj)
{
    functional_ = obj;
    child_->setTitle(category + "/" + functional_->getId());
    id_box_->setText(std::to_string(functional_->getUniqueId()));
    act_box_->setText(functional_->getActivation());
    fun_box_->setText(functional_->getFunctionsStr());
    data_box_->setText(functional_->getDatasStr());

    button_->connect("pressed", [this]() {
        this->functional_->setActivation(this->act_box_->getText());
        this->functional_->setFunctionsStr(this->fun_box_->getText());
        this->functional_->setDatasStr(this->data_box_->getText());
        child_->close();
    });
}

