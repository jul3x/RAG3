//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/ObstacleObjectWindow.h>


using namespace editor;

ObstacleObjectWindow::ObstacleObjectWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Special editor",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size"),
                    "obstacle_object_window"),
        obstacle_(nullptr),
        functions_(theme, this)
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "20");
    grid_->setSize("90%", "100% - 20");
    grid_->setAutoSize(false);
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

    grid_->addWidget(functions_.getGrid(), 4, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));

    grid_->addWidget(button_, 5, 0);
}

void ObstacleObjectWindow::setObjectContent(const std::string& category, Obstacle* obj)
{
    this->setSize(CONF<sf::Vector2f>("popup_window_size"));

    obstacle_ = obj;
    child_->setTitle(category + "/" + obstacle_->getId());
    id_box_->setText(std::to_string(obstacle_->getUniqueId()));
    act_box_->setText(obstacle_->getActivationStr());
    functions_.setObjectContent(obj);

    button_->disconnectAll();
    button_->connect("pressed", [this]() {
        this->obstacle_->setFunctionsStr(this->functions_.getFunctionsStr());
        this->obstacle_->setDatasStr(this->functions_.getDatasStr());
        this->obstacle_->setActivationStr(this->act_box_->getText());
        child_->close();
    });
}

bool ObstacleObjectWindow::isDataFocused() const
{
    return functions_.isDataFocused();
}

void ObstacleObjectWindow::addToData(const std::string& str)
{
    return functions_.addToData(str);
}


