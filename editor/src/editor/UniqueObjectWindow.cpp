//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Config.h>

#include <editor/UniqueObjectWindow.h>


using namespace editor;

UniqueObjectWindow::UniqueObjectWindow(tgui::Gui* gui, tgui::Theme* theme) :
        ChildWindow(gui, theme, "Unique identifier of object",
                    (sf::Vector2f(CONF<int>("window_width_px"), CONF<int>("window_heigth_px")) -
                     CONF<sf::Vector2f>("unique_object_window_size")) / 2.0f,
                    CONF<sf::Vector2f>("unique_object_window_size"),
                    "unique_object_window")
{
    grid_ = tgui::Grid::create();
    grid_->setPosition("50% - width/2", "50% - height/2");
    grid_->setSize("100%", "100%");
    child_->add(grid_);

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Identifier:");
    label->setTextSize(CONF<float>("label_text_size"));

    grid_->addWidget(label, 0, 0);

    text_box_ = tgui::EditBox::create();
    text_box_->setRenderer(theme_->getRenderer("EditBox"));
    text_box_->setReadOnly(true);
    text_box_->setSize("70%", CONF<float>("text_box_height"));
    text_box_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(text_box_, 1, 0);

    grid_->setWidgetPadding(0, 0, {CONF<float>("items_padding"), CONF<float>("items_padding")});
    grid_->setWidgetPadding(1, 0, {CONF<float>("items_padding"), CONF<float>("items_padding")});
}

void UniqueObjectWindow::setObjectContent(const std::string& category, const std::string& name, int id)
{
    child_->setTitle(category + "/" + name);
    text_box_->setText(std::to_string(id));
}

