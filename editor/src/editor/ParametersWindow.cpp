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
    grid_->setSize("90%", "50%");
    grid_->setAutoSize(true);
    child_->add(grid_);

    float padding = CONF<float>("items_padding");

    int i = 0;
    for (const auto& parameter : map_parameters)
    {
        auto label = tgui::Label::create();
        label->setRenderer(theme_->getRenderer("Label"));
        label->setText(parameter);
        label->setTextSize(CONF<float>("label_text_size"));
        grid_->addWidget(label, i, 0);
        ++i;

        auto edit_box = tgui::EditBox::create();
        edit_box->setRenderer(theme->getRenderer("EditBox"));
        edit_box->setSize("80%", CONF<float>("text_box_height"));
        edit_box->setTextSize(CONF<float>("label_text_size"));
        grid_->addWidget(edit_box, i, 0);
        ++i;

        grid_->setWidgetPadding(i, 0, {padding, padding});
        parameters_boxes_.emplace(std::make_pair(parameter, edit_box));
    }

    button_ = tgui::Button::create();
    button_->setRenderer(theme_->getRenderer("Button"));
    button_->setText("Save");
    button_->setTextSize(CONF<float>("label_text_size"));
    button_->setSize(CONF<sf::Vector2f>("button_size"));
    button_->setPosition("50% - width/2", "100% - " + std::to_string(CONF<float>("button_relative_valign")));

    child_->add(button_);
}

void ParametersWindow::setParameters()
{
    //todo resolve the problem with type mismatch
    auto& params = Editor::get().getMap().getParams();

    for (const auto& parameter : map_parameters)
    {
        parameters_boxes_[parameter]->setText(j3x::get<std::string>(params, parameter));
    }

    button_->connect("pressed", [&]() {
        j3x::Parameters params;

        for (const auto& parameter : map_parameters)
        {
            params[parameter] = std::string(parameters_boxes_[parameter]->getText());
        }

        Editor::get().getMap().setParams(params);

        child_->close();
    });
}
