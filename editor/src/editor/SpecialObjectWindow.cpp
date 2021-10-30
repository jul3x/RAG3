//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/SpecialObjectWindow.h>


using namespace editor;

SpecialObjectWindow::SpecialObjectWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Special editor",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size"),
                    "special_object_window"),
        special_(nullptr),
        functions_(theme)
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "20");
    grid_->setSize("90%", "80%");
    grid_->setAutoSize(true);
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Identifier:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    id_box_ = tgui::EditBox::create();
    id_box_->setRenderer(theme_->getRenderer("EditBox"));
    id_box_->setReadOnly(true);
    id_box_->setSize("35%", CONF<float>("text_box_height"));
    id_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(id_box_, 1, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Activate:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 2, 0);

    act_box_ = tgui::EditBox::create();
    act_box_->setRenderer(theme_->getRenderer("EditBox"));
    act_box_->setSize("35%", CONF<float>("text_box_height"));
    act_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(act_box_, 3, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Already active:");
    label->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(label, 4, 0);

    already_active_ = tgui::EditBox::create();
    already_active_->setRenderer(theme_->getRenderer("EditBox"));
    already_active_->setSize("35%", CONF<float>("text_box_height"));
    already_active_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(already_active_, 5, 0);

    grid_->addWidget(functions_.getGrid(), 6, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));

    grid_->addWidget(button_, 7, 0);
}

void SpecialObjectWindow::setObjectContent(const std::string& category, Special* obj)
{
    special_ = obj;
    child_->setTitle(category + "/" + special_->getId());
    id_box_->setText(std::to_string(special_->getUniqueId()));
    act_box_->setText(special_->getActivationStr());
    already_active_->setText(std::to_string(special_->isActive()));
    functions_.setObjectContent(obj);

    button_->connect("pressed", [this]() {
        this->special_->setActivationStr(this->act_box_->getText());

        this->special_->setFunctionsStr(this->functions_.getFunctionsStr());
        this->special_->setDatasStr(this->functions_.getDatasStr());

        if (already_active_->getText() == "1")
        {
            this->special_->activate();
        }
        else
        {
            this->special_->deactivate();
        }

        child_->close();
    });
}

bool SpecialObjectWindow::isDataFocused() const
{
    return functions_.isDataFocused();
}

void SpecialObjectWindow::addToData(const std::string& str)
{
    return functions_.addToData(str);
}


