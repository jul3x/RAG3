//
// Created by jul3x on 15.08.21.
//

#include <R3E/system/Config.h>

#include <common/Framework.h>

#include <ui/menu/SettingsWindow.h>


SettingsWindow::SettingsWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework) :
        MenuWindow(gui, theme, "> Settings"),
        framework_(framework), theme_(theme), gui_(gui),
        focused_controls_widget_(nullptr)
{
    auto pos = CONF<sf::Vector2f>("graphics/menu_window_pos") +
               sf::Vector2f((CONF<float>("graphics/menu_bar_width_px") + CONF<int>("graphics/window_width_px") -
                             CONF<sf::Vector2f>("graphics/menu_window_size").x) / 2.0f, 0.0f);

    pos = {pos.x, pos.y + 0.2f * CONF<sf::Vector2f>("graphics/menu_window_size").y};
    tabs_ = tgui::Tabs::create();
    tabs_->setRenderer(theme->getRenderer("TabsGame"));
    tabs_->getRenderer()->setFont(RM.getFont("default"));
    tabs_->setTabHeight(CONF<float>("graphics/menu_settings_tabs_height"));
    tabs_->setPosition(pos - sf::Vector2f(0.0, CONF<float>("graphics/menu_settings_tabs_height")));
    tabs_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    tabs_->getRenderer()->setDistanceToSide(20.0f * CONF<float>("graphics/user_interface_zoom"));
    for (const auto& tab : {"General", "Controls", "Graphics", "Sound"})
    {
        tabs_->add(tab);
        tab_names_.emplace_back(tab);
    }

    save_button_ = tgui::Button::create("Save");
    save_button_->setRenderer(theme->getRenderer("ButtonGame"));
    save_button_->getRenderer()->setFont(RM.getFont("default"));
    save_button_->setPosition(pos - sf::Vector2f(0.0, CONF<float>("graphics/menu_settings_tabs_height")) +
                              CONF<sf::Vector2f>("graphics/menu_save_button_pos"));
    save_button_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    save_button_->connect("pressed", [this, framework]() {
        this->unfocusControlsWidgets();
        this->saveValues();
        framework->spawnSound(RM.getSound("ui_upgrade"), framework->getPlayer()->getPosition());
    });
    save_button_->connect("mouseentered", [framework]() {
        framework->spawnSound(RM.getSound("ui_hover"), framework->getPlayer()->getPosition());
    });
    gui->add(save_button_);

    restore_button_ = tgui::Button::create("Restore");
    restore_button_->setRenderer(theme->getRenderer("ButtonGame"));
    restore_button_->getRenderer()->setFont(RM.getFont("default"));
    restore_button_->setPosition(pos - sf::Vector2f(0.0, CONF<float>("graphics/menu_settings_tabs_height")) +
                                 CONF<sf::Vector2f>("graphics/menu_restore_button_pos"));
    restore_button_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    restore_button_->connect("pressed", [this, framework]() {
        this->unfocusControlsWidgets();
        this->updateValues();
        framework->spawnSound(RM.getSound("ui_upgrade"), framework->getPlayer()->getPosition());
    });
    gui->add(restore_button_);

    scroll_panel_ = tgui::ScrollablePanel::create({CONF<sf::Vector2f>("graphics/menu_window_size").x,
                                                   0.8f * CONF<sf::Vector2f>("graphics/menu_window_size").y});
    scroll_panel_->setPosition(pos);
    scroll_panel_->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
    scroll_panel_->setVerticalScrollAmount(100);
    scroll_panel_->setRenderer(theme->getRenderer("ScrollablePanelGame"));
    scroll_panel_->getRenderer()->setScrollbarWidth(CONF<float>("graphics/menu_scrollbar_width"));
    scroll_panel_->getRenderer()->setPadding(tgui::Padding(0.0f, CONF<float>("graphics/menu_items_padding")));

    createWidgets();

    gui->add(scroll_panel_);
    gui->add(tabs_);

    tabs_->connect("TabSelected", [this, gui, framework](const std::string& tab) {
        this->onTabSelected(*gui, tab);
        framework->spawnSound(RM.getSound("ui_hover"), framework->getPlayer()->getPosition());
    });
    tabs_->select(0);

    doShow(false);
}

void SettingsWindow::doShow(bool show)
{
    MenuWindow::doShow(show);
    save_button_->setVisible(show);
    restore_button_->setVisible(show);
    tabs_->setVisible(show);
    scroll_panel_->setVisible(show);
    unfocusControlsWidgets();
}

void SettingsWindow::createWidgets()
{
    for (const auto& tab : tab_names_)
    {
        grids_.emplace_back();
        grids_.back() = tgui::Grid::create();
        grids_.back()->setAutoSize(true);

        scroll_panel_->add(grids_.back(), tab);

        auto filename = CONF<std::string>("paths/user_dir") + "/" + r3e::utils::toLower(tab) + ".j3x";
        params_[tab] = r3e::j3x::parseWithVisitor(filename, "");

        int i = 0;

        const auto& types = params_[tab]->getTypes();
        const auto& values = params_[tab]->getParams();

        bool is_normal_widget = tab == "Graphics" || tab == "Sound";
        bool is_control_widget = tab == "Controls";
        for (const auto& param : params_[tab]->getVariablesList())
        {
            auto label = tgui::Label::create(r3e::utils::humanize(param));
            label->setRenderer(theme_->getRenderer("ItemLabel"));
            label->setTextSize(CONF<float>("graphics/menu_window_text_size"));
            label->getRenderer()->setFont(RM.getFont("default"));

            tgui::Widget::Ptr widget;
            if (is_normal_widget)
                widget = this->createWidget(values, param, types.at(param), i);
            else if (is_control_widget)
                widget = this->createControlsWidget(values, param);
            else
                widget = this->createGeneralWidget(values, param, types.at(param), i);

            grids_.back()->addWidget(label, i, 0);
            grids_.back()->addWidget(widget, i, 1);

            grids_.back()->setWidgetPadding(label, {CONF<float>("graphics/menu_items_padding"), 0.0f});
            grids_.back()->setWidgetPadding(widget, {CONF<float>("graphics/menu_items_padding"), 0.0f});
            grids_.back()->setWidgetAlignment(label, tgui::Grid::Alignment::BottomLeft);
            grids_.back()->setWidgetAlignment(widget, is_normal_widget ?
                                                      tgui::Grid::Alignment::Bottom :
                                                      tgui::Grid::Alignment::BottomLeft);
            ++i;
        }
    }
}

void SettingsWindow::updateValues()
{
    int i = 0;
    for (const auto& tab : tab_names_)
    {
        int j = 0;
        const auto& types = params_[tab]->getTypes();
        const auto& values = params_[tab]->getParams();
        for (const auto& param : params_[tab]->getVariablesList())
        {
            auto* widget = grids_[i]->getWidget(j, 1).get();

            if (tab == "Controls")
            {
                dynamic_cast<tgui::Button*>(widget)->setText(r3e::utils::keyToString(
                        static_cast<sf::Keyboard::Key>(r3e::j3x::get<int>(values, param))));
            }
            else if (types.at(param) == "bool")
            {
                dynamic_cast<tgui::CheckBox*>(widget)->setChecked(r3e::j3x::get<bool>(values, param));
            }
            else if (types.at(param) == "int")
            {
                dynamic_cast<tgui::Slider*>(widget)->setValue(r3e::j3x::get<int>(values, param));
            }
            else if (types.at(param) == "float")
            {
                dynamic_cast<tgui::Slider*>(widget)->setValue(r3e::j3x::get<float>(values, param));
            }
            else if (types.at(param) == "string")
            {
                dynamic_cast<tgui::EditBox*>(widget)->setText(r3e::j3x::get<std::string>(values, param));
            }
            ++j;
        }
        ++i;
    }
}

void SettingsWindow::saveValues()
{
    int i = 0;
    for (const auto& tab : tab_names_)
    {
        int j = 0;
        const auto& types = params_[tab]->getTypes();
        std::string out;
        for (const auto& param : params_[tab]->getVariablesList())
        {
            auto* widget = grids_[i]->getWidget(j, 1).get();
            if (tab == "Controls")
            {
                j3x::serializeAssign(param, static_cast<int>(
                        r3e::utils::stringToKey(dynamic_cast<tgui::Button*>(widget)->getText())), out);
            }
            else if (types.at(param) == "bool")
            {
                j3x::serializeAssign(param, dynamic_cast<tgui::CheckBox*>(widget)->isChecked(), out);
            }
            else if (types.at(param) == "int")
            {
                j3x::serializeAssign(param, static_cast<int>(dynamic_cast<tgui::Slider*>(widget)->getValue()), out);
            }
            else if (types.at(param) == "float")
            {
                j3x::serializeAssign(param, dynamic_cast<tgui::Slider*>(widget)->getValue(), out);
            }
            else if (types.at(param) == "string")
            {
                j3x::serializeAssign(param, std::string(dynamic_cast<tgui::EditBox*>(widget)->getText()), out);
            }
            ++j;
        }

        RM.saveConfigFile("user_dir", r3e::utils::toLower(tab), out);
        ++i;
    }
}

tgui::Widget::Ptr
SettingsWindow::createWidget(const j3x::Parameters& values, const std::string& param, const std::string& type, int i)
{
    tgui::Widget::Ptr widget;
    if (type == "bool")
    {
        auto checkbox = tgui::CheckBox::create("");
        checkbox->setChecked(r3e::j3x::get<bool>(values, param));
        widget = checkbox;
        widget->setRenderer(theme_->getRenderer("CheckBoxGame"));
        widget->setSize({CONF<sf::Vector2f>("graphics/menu_checkbox_size")});
    }
    else if (type == "int" || type == "float")
    {
        bool is_int = type == "int";
        auto param_min_str = "ranges/" + param + "_min";
        auto param_max_str = "ranges/" + param + "_max";
        auto slider = tgui::Slider::create();
        auto value = tgui::Label::create();

        if (is_int)
        {
            slider->setMinimum(CONF<int>(param_min_str));
            slider->setMaximum(CONF<int>(param_max_str));
            slider->setValue(r3e::j3x::get<int>(values, param));
            value->setText(std::to_string(r3e::j3x::get<int>(values, param)));
        }
        else
        {
            slider->setStep(0.1f);
            slider->setMinimum(CONF<float>(param_min_str));
            slider->setMaximum(CONF<float>(param_max_str));
            slider->setValue(r3e::j3x::get<float>(values, param));
            value->setText(r3e::utils::floatToString(r3e::j3x::get<float>(values, param)));
        }

        value->setRenderer(theme_->getRenderer("ItemLabel"));
        value->setTextSize(CONF<float>("graphics/menu_window_text_size"));
        value->getRenderer()->setFont(RM.getFont("default"));

        slider->connect("ValueChanged", [value, slider, is_int]() {
            if (is_int)
                value->setText(std::to_string(static_cast<int>(slider->getValue())));
            else
                value->setText(r3e::utils::floatToString(slider->getValue()));
        });

        slider->setRenderer(theme_->getRenderer("SliderGame"));
        slider->setSize(CONF<sf::Vector2f>("graphics/menu_slider_size"));
        widget = slider;

        grids_.back()->addWidget(value, i, 2);
        grids_.back()->setWidgetAlignment(value, tgui::Grid::Alignment::BottomLeft);
    }
    else if (type == "string")
    {
        auto edit_box = tgui::EditBox::create();
        edit_box->setText(r3e::j3x::get<std::string>(values, param));
        edit_box->setTextSize(CONF<float>("graphics/menu_window_text_size"));
        edit_box->setSize(CONF<sf::Vector2f>("graphics/menu_edit_box_size"));
        widget = edit_box;
        widget->setRenderer(theme_->getRenderer("EditBoxGame"));
        widget->getRenderer()->setFont(RM.getFont("default"));
    }
    return widget;
}

tgui::Widget::Ptr SettingsWindow::createControlsWidget(const j3x::Parameters& values, const std::string& param)
{
    auto widget = tgui::Button::create(r3e::utils::keyToString(
            static_cast<sf::Keyboard::Key>(r3e::j3x::get<int>(values, param))));
    widget->setRenderer(theme_->getRenderer("ButtonLabel"));
    widget->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    widget->getRenderer()->setFont(RM.getFont("default"));
    clicked_button_color_ = widget->getRenderer()->getTextColorDown();
    normal_button_color_ = widget->getRenderer()->getTextColor();

    widget->setFocusable(true);
    controls_widgets_.emplace_back(widget.get());

    widget->connect("mousepressed", [widget, this]() {
        this->unfocusControlsWidgets();
        focused_controls_widget_ = widget.get();
        focused_controls_widget_->getRenderer()->setTextColor(clicked_button_color_);
    });

    return widget;
}

void SettingsWindow::unfocusControlsWidgets()
{
    if (focused_controls_widget_ != nullptr)
        focused_controls_widget_->getRenderer()->setTextColor(normal_button_color_);
    focused_controls_widget_ = nullptr;
}

void SettingsWindow::setKey(sf::Keyboard::Key k)
{
    static const std::unordered_set<sf::Keyboard::Key> forbidden_keys =
            {sf::Keyboard::Key::Unknown, sf::Keyboard::Key::Escape};
    if (!utils::contains(forbidden_keys, k) && focused_controls_widget_ != nullptr)
    {
        focused_controls_widget_->setText(r3e::utils::keyToString(k));
        unfocusControlsWidgets();
        framework_->spawnSound(RM.getSound("ui_hover"), framework_->getPlayer()->getPosition());
    }
}

tgui::Widget::Ptr
SettingsWindow::createGeneralWidget(const j3x::Parameters& values, const std::string& param, const std::string& type,
                                    int i)
{
//    if (param == "player_name")
    return this->createWidget(values, param, type, i);

}
