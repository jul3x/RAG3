//
// Created by jul3x on 16.03.20.
//

#include <R3E/system/Config.h>

#include <editor/MenuWindow.h>
#include <editor/UserInterface.h>
#include <Editor.h>


using namespace editor;

MenuWindow::MenuWindow(UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme) :
        ui_(ui),
        ChildWindow(gui, theme, "Menu",
                    CONF<sf::Vector2f>("menu_window_pos"),
                    CONF<sf::Vector2f>("menu_window_size"),
                    "menu_window")
{
    grid_ = tgui::Grid::create();
    grid_->setSize(CONF<sf::Vector2f>("menu_window_size") * 0.85f);
    grid_->setPosition(CONF<sf::Vector2f>("menu_window_size") * 0.075f);
    child_->add(grid_);


    auto create_button = [this](const std::string& name, const std::function<void()>& function, int i, int j) {
        auto button = tgui::Button::create();
        button->setRenderer(theme_->getRenderer("Button"));
        auto button_size = CONF<sf::Vector2f>("button_size");
        unsigned int button_text_size = CONF<float>("label_text_size");
        button->setSize(button_size);
        button->setText(name);
        button->setTextSize(button_text_size);
        button->connect("Clicked", function);
        grid_->addWidget(button, i, j);
    };

    create_button("Load map", [this]() {
        this->ui_->resetMapList();
        this->gui_->get("load_window")->setVisible(true);
    }, 0, 0);

    create_button("Save map", [this]() {
        this->ui_->resetMapList();
        this->gui_->get("save_window")->setVisible(true);
    }, 0, 1);

    create_button("Edit graphics", [this]() {
        this->ui_->openConfigWindow("config_dir", "graphics");
    }, 1, 0);

    create_button("Edit config", [this]() {
        this->ui_->openConfigWindow("config_dir", "config");
    }, 1, 1);

    create_button("Edit characters", [this]() {
        this->ui_->openConfigWindow("config_dir", "characters");
    }, 2, 0);

    create_button("Edit sound", [this]() {
        this->ui_->openConfigWindow("config_dir", "sound");
    }, 2, 1);

    create_button("Edit parameters", [this]() {
        this->ui_->openParametersWindow();
    }, 3, 0);

    z_index_label_ = tgui::Label::create();
    z_index_label_->setRenderer(theme_->getRenderer("Label"));
    z_index_label_->setText("Visible z-index: 10");
    z_index_label_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(z_index_label_, 4, 0);

    z_index_slider_ = tgui::Slider::create();
    z_index_slider_->setRenderer(theme_->getRenderer("Slider"));
    z_index_slider_->setSize(CONF<sf::Vector2f>("menu_slider_size"));
    z_index_slider_->setValue(10);
    z_index_slider_->setMinimum(-10);
    z_index_slider_->setMaximum(10);
    grid_->addWidget(z_index_slider_, 4, 1);

    this->ui_->setZIndex(10);

    z_index_slider_->connect("ValueChanged", [this]() {
        int value = this->z_index_slider_->getValue();
        this->z_index_label_->setText("Visible z-index: " + std::to_string(value));
        this->ui_->setZIndex(value);
    });

    random_label_ = tgui::Label::create();
    random_label_->setRenderer(theme_->getRenderer("Label"));
    random_label_->setText("Randomizing tiles: 0");
    random_label_->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(random_label_, 5, 0);

    random_slider_ = tgui::Slider::create();
    random_slider_->setRenderer(theme_->getRenderer("Slider"));
    random_slider_->setSize(CONF<sf::Vector2f>("menu_slider_size"));
    random_slider_->setValue(0);
    random_slider_->setMinimum(0);
    random_slider_->setMaximum(9);
    grid_->addWidget(random_slider_, 5, 1);

    Editor::get().setRandomizing(0);

    random_slider_->connect("ValueChanged", [this]() {
        int value = this->random_slider_->getValue();
        this->random_label_->setText("Randomizing tiles: " + std::to_string(value));
        Editor::get().setRandomizing(value);
    });

    auto label = tgui::Label::create();
    label->setRenderer(theme_->getRenderer("Label"));
    label->setText("Lighting");
    label->setTextSize(CONF<float>("label_text_size"));
    grid_->addWidget(label, 6, 0);

    auto checkbox = tgui::CheckBox::create();
    checkbox->setRenderer(theme_->getRenderer("CheckBox"));
    checkbox->setSize(CONF<float>("checkbox_size"), CONF<float>("checkbox_size"));
    checkbox->setChecked(true);
    grid_->addWidget(checkbox, 6, 1);

    checkbox->connect("Checked", [&]() {
        Editor::get().setLighting(true);
    });
    checkbox->connect("Unchecked", [&]() {
        Editor::get().setLighting(false);
    });
}
