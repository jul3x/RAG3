//
// Created by jul3x on 09.05.20.
//

#include <R3E/system/Config.h>

#include <common/Character.h>

#include <editor/CharacterObjectWindow.h>


using namespace editor;

CharacterObjectWindow::CharacterObjectWindow(tgui::Gui *gui, tgui::Theme *theme) :
        ChildWindow(gui, theme, "Character editor",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_heigth_px")) -
                     CONF<sf::Vector2f>("character_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("character_window_size"),
                    "character_object_window"),
        character_(nullptr) {
    float padding = CONF<float>("items_padding");

    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "30% - height/2");
    grid_->setSize("90%", "40%");
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

    grid_->addWidget(label, 0, 1);

    act_box_ = tgui::EditBox::create();
    act_box_->setRenderer(theme_->getRenderer("EditBox"));
    act_box_->setSize("35%", CONF<float>("text_box_height"));
    act_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(act_box_, 1, 1);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Function:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 2, 0);

    fun_box_ = tgui::TextBox::create();
    fun_box_->setRenderer(theme_->getRenderer("TextBox"));
    fun_box_->setSize("35%", "45%");
    fun_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(fun_box_, 3, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Function data:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 2, 1);

    data_box_ = tgui::TextBox::create();
    data_box_->setRenderer(theme_->getRenderer("TextBox"));
    data_box_->setSize("35%", "45%");
    data_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(data_box_, 3, 1);


    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Conversation:");
    label->setTextSize(CONF<float>("label_text_size"));
    label->setPosition("50% - width/2", "50%");
    child_->add(label);

    talk_box_ = tgui::TextBox::create();
    talk_box_->setRenderer(theme_->getRenderer("TextBox"));
    talk_box_->setSize("90% - " + std::to_string(padding * 2.0f), "35%");
    talk_box_->setPosition("50% - width/2", "55%");
    talk_box_->setTextSize(CONF<float>("label_text_size"));
    child_->add(talk_box_);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));
    button_->setPosition("50% - width/2", "100% - " + std::to_string(CONF<float>("button_relative_valign")));

    child_->add(button_);

    grid_->setWidgetPadding(0, 0, {padding, padding});
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(0, 1, {padding, padding});
    grid_->setWidgetPadding(1, 1, {padding, padding});
    grid_->setWidgetPadding(2, 0, {padding, padding});
    grid_->setWidgetPadding(3, 0, {padding, padding});
    grid_->setWidgetPadding(2, 1, {padding, padding});
    grid_->setWidgetPadding(3, 1, {padding, padding});
}

void CharacterObjectWindow::setObjectContent(const std::string &category, Character *obj) {
    character_ = obj;
    child_->setTitle(category + "/" + character_->getId());
    id_box_->setText(std::to_string(character_->getUniqueId()));
    act_box_->setText(character_->getActivationStr());
    fun_box_->setText(character_->getFunctionsStr());
    data_box_->setText(character_->getDatasStr());

    if (!character_->isTalkable()) {
        talk_box_->setEnabled(false);
        talk_box_->setText("No conversation");
    } else {
        talk_box_->setEnabled(true);
        talk_box_->setText(character_->getTalkScenarioStr());
    }

    button_->connect("pressed", [this]() {
        this->character_->setActivationStr(this->act_box_->getText());
        this->character_->setFunctionsStr(this->fun_box_->getText());
        this->character_->setDatasStr(this->data_box_->getText());

        if (this->character_->isTalkable())
            this->character_->setTalkScenarioStr(this->talk_box_->getText());
        child_->close();
    });
}

bool CharacterObjectWindow::isDataFocused() const {
    return data_box_->isFocused();
}

void CharacterObjectWindow::addToData(const std::string &str) {
    data_box_->setText(data_box_->getText() + str);
}


