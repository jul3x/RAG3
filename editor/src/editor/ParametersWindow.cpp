//
// Created by macie on 10/08/2021.
//

#include <R3E/system/Config.h>

#include <editor/ParametersWindow.h>

#include <Editor.h>


using namespace editor;

ParametersWindow::ParametersWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Parameters editor",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_height_px")) -
                     CONF<sf::Vector2f>("popup_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("popup_window_size"),
                    "parameters_window")
{
    child_->setTitle("Map parameters");

    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "20");
    grid_->setSize("90%", "80%");
    grid_->setAutoSize(true);
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Shader:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    shader_box_ = tgui::EditBox::create();
    shader_box_->setRenderer(theme_->getRenderer("EditBox"));
    shader_box_->setSize("35%", CONF<float>("text_box_height"));
    shader_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(shader_box_, 1, 0);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Background color");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 1);

    background_box_ = tgui::EditBox::create();
    background_box_->setRenderer(theme_->getRenderer("EditBox"));
    background_box_->setSize("35%", CONF<float>("text_box_height"));
    background_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(background_box_, 1, 1);

    label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Lighting color");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 2, 0);

    lighting_box_ = tgui::EditBox::create();
    lighting_box_->setRenderer(theme_->getRenderer("EditBox"));
    lighting_box_->setSize("35%", CONF<float>("text_box_height"));
    lighting_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(lighting_box_, 3, 0);

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));
    button_->setPosition("50% - width/2", "100% - " + std::to_string(CONF<float>("button_relative_valign")));

    child_->add(button_);

    float padding = CONF<float>("items_padding");
    grid_->setWidgetPadding(1, 0, {padding, padding});
    grid_->setWidgetPadding(1, 1, {padding, padding});
    grid_->setWidgetPadding(3, 0, {padding, padding});
}

void ParametersWindow::setParameters()
{
    auto& params = Editor::get().getMap().getParams();

    shader_box_->setText(j3x::get<std::string>(params, "shader"));
    background_box_->setText(std::to_string(j3x::get<int>(params, "background_color")));
    lighting_box_->setText(std::to_string(j3x::get<int>(params, "lightning_color")));

    button_->connect("pressed", [&]() {
        j3x::Parameters params;
        params["shader"] = std::string(shader_box_->getText());
        params["background_color"] = std::stoi(std::string(background_box_->getText()));
        params["lightning_color"] = std::stoi(std::string(lighting_box_->getText()));

        Editor::get().getMap().setParams(params);

        child_->close();
    });
}
