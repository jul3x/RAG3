//
// Created by jul3x on 15.08.21.
//

#include <R3E/system/Config.h>

#include <common/Framework.h>

#include <common/ui/menu/SettingsWindow.h>


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
    for (const auto& tab : {"general", "controls", "graphics", "sound"})
    {
        tabs_->add(utils::humanize(tab));
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
        framework->spawnSound(RM.getSound("ui_upgrade"));
    });
    save_button_->connect("mouseentered", [framework]() {
        framework->spawnSound(RM.getSound("ui_hover"));
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
        this->saveValues();
        framework->spawnSound(RM.getSound("ui_upgrade"));
    });
    restore_button_->connect("mouseentered", [framework]() {
        framework->spawnSound(RM.getSound("ui_hover"));
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
    updateValues();

    gui->add(scroll_panel_);
    gui->add(tabs_);

    tabs_->connect("TabSelected", [this, gui, framework](const std::string& tab) {
        this->onTabSelected(*gui, tab);
        framework->spawnSound(RM.getSound("ui_hover"));
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
        auto grid = tgui::Grid::create();
        grid->setAutoSize(true);

        scroll_panel_->add(grid, utils::humanize(tab));

        auto filename = CONF<std::string>("paths/user_dir") + "/" + tab + ".j3x";
        params_[tab] = r3e::j3x::parseWithVisitor(filename, "");

        int i = 0;

        const auto& types = params_[tab]->getTypes();
        const auto& values = params_[tab]->getParams();

        for (const auto& param : params_[tab]->getVariablesList())
        {
            auto hash_key = tab + "/" + param;
            const auto& type = types.at(param);

            if (tab == "general" && param == "character")
                widgets_[hash_key] = std::make_unique<CharacterSettingsWidget>(theme_, tab, param);
            else if (tab == "controls")
                widgets_[hash_key] = std::make_unique<ControlsSettingsWidget>(this, theme_, tab, param);
            else
            {
                if (type == "bool")
                    widgets_[hash_key] = std::make_unique<BoolSettingsWidget>(theme_, tab, param);
                else if (type == "int" && utils::endsWith(param, "color"))
                    widgets_[hash_key] = std::make_unique<ColorSettingsWidget>(theme_, tab, param);
                else if (type == "int")
                    widgets_[hash_key] = std::make_unique<SliderSettingsWidget<int>>(theme_, tab, param);
                else if (type == "float")
                    widgets_[hash_key] = std::make_unique<SliderSettingsWidget<float>>(theme_, tab, param);
                else if (type == "string")
                    widgets_[hash_key] = std::make_unique<StringSettingsWidget>(theme_, tab, param);
                else
                    throw std::runtime_error(
                            "[SettingsWindow::createWidgets] Widgets for type " + type + " not handled yet!");
            }

            grid->addWidget(widgets_[hash_key]->getLabel(), i, 0);
            grid->addWidget(widgets_[hash_key]->getWidget(), i, 1);
            grid->setWidgetPadding(widgets_[hash_key]->getLabel(), {CONF<float>("graphics/menu_items_padding"), 0.0f});
            grid->setWidgetPadding(widgets_[hash_key]->getWidget(), {CONF<float>("graphics/menu_items_padding"), 0.0f});
            grid->setWidgetAlignment(widgets_[hash_key]->getLabel(), tgui::Grid::Alignment::Left);
            grid->setWidgetAlignment(widgets_[hash_key]->getWidget(), widgets_[hash_key]->getAlignment());
            ++i;
        }
    }
}

void SettingsWindow::updateValues()
{
    for (const auto& tab : tab_names_)
    {
        for (const auto& param : params_[tab]->getVariablesList())
        {
            widgets_[tab + "/" + param]->updateValue(CFG.getParameters());
        }
    }
}

void SettingsWindow::saveValues()
{
    bool should_reload_game = false;
    bool should_reload_sound = false;

    auto buffer = j3x::Parameters();
    const auto& current_parameters = CFG.getParameters();
    for (const auto& tab : tab_names_)
    {
        std::string out;
        for (const auto& param : params_[tab]->getVariablesList())
        {
            auto hash_key = tab + "/" + param;
            widgets_[hash_key]->serializeAndAppend(out);
            auto game_reload_widget = utils::contains(RELOAD_PARAMETERS, hash_key);
            auto sound_reload_widget = utils::contains(SOUND_RELOAD_PARAMETERS, hash_key);

            if (game_reload_widget || sound_reload_widget)
            {
                if (!j3x::compare(widgets_[hash_key]->getValue(), current_parameters.at(hash_key)))
                {
                    if (game_reload_widget)
                    {
                        should_reload_game = true;
                        buffer[hash_key] = current_parameters.at(hash_key);
                    }
                    if (sound_reload_widget)
                        should_reload_sound = true;
                }
            }
        }

        RM.saveConfigFile("user_dir", tab, out);
        CFG.appendConfig("../data/config/user/" + tab + ".j3x", tab, true);
    }

    // Revoke parameters that forces game to be reloaded
    if (should_reload_game)
    {
        for (const auto& param : buffer)
        {
            CFG.set(param.first, param.second);
        }
        framework_->getUI()
                  ->spawnNoteWindow("Graphics settings which controls the game window\nwill be applied after restart.",
                                    false);
    }

    framework_->initParticles();
    framework_->initSound(should_reload_sound);
}

void SettingsWindow::unfocusControlsWidgets()
{
    if (focused_controls_widget_ != nullptr)
        focused_controls_widget_->setFocus(false);
    focused_controls_widget_ = nullptr;
}

void SettingsWindow::setKey(sf::Keyboard::Key k)
{
    static const std::unordered_set<sf::Keyboard::Key> forbidden_keys =
            {sf::Keyboard::Key::Unknown, sf::Keyboard::Key::Escape};
    if (!utils::contains(forbidden_keys, k) && focused_controls_widget_ != nullptr)
    {
        focused_controls_widget_->setKey(k);
        framework_->spawnSound(RM.getSound("ui_hover"));
    }

    unfocusControlsWidgets();
}

void SettingsWindow::setFocusedControlsWidget(ControlsSettingsWidget* widget)
{
    focused_controls_widget_ = widget;
    widget->setFocus(true);
}


/* Widgets */

BaseSettingsWidget::BaseSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        name_(name), tab_(tab)
{
    label_ = tgui::Label::create(r3e::utils::humanize(name));
    label_->setRenderer(theme->getRenderer("ItemLabel"));
    label_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    label_->getRenderer()->setFont(RM.getFont("default"));
}

[[nodiscard]] tgui::Label::Ptr BaseSettingsWidget::getLabel() const
{
    return label_;
}

[[nodiscard]] tgui::Grid::Alignment BaseSettingsWidget::getAlignment() const
{
    return tgui::Grid::Alignment::BottomLeft;
}

BoolSettingsWidget::BoolSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    checkbox_ = tgui::CheckBox::create("");
    checkbox_->setRenderer(theme->getRenderer("CheckBoxGame"));
    checkbox_->setSize({CONF<sf::Vector2f>("graphics/menu_checkbox_size")});
}

[[nodiscard]] tgui::Widget::Ptr BoolSettingsWidget::getWidget() const
{
    return checkbox_;
}

void BoolSettingsWidget::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, checkbox_->isChecked(), out);
}

void BoolSettingsWidget::updateValue(const j3x::Parameters& values)
{
    checkbox_->setChecked(r3e::j3x::get<bool>(values, tab_ + "/" + name_));
}

j3x::Obj BoolSettingsWidget::getValue() const
{
    return checkbox_->isChecked();
}


StringSettingsWidget::StringSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    editbox_ = tgui::EditBox::create();
    editbox_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    editbox_->setSize(CONF<sf::Vector2f>("graphics/menu_edit_box_size"));
    editbox_->setRenderer(theme->getRenderer("EditBoxGame"));
    editbox_->getRenderer()->setFont(RM.getFont("default"));
    editbox_->setMaximumCharacters(100);
}

[[nodiscard]] tgui::Widget::Ptr StringSettingsWidget::getWidget() const
{
    return editbox_;
}

void StringSettingsWidget::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, std::string(editbox_->getText()), out);
}

void StringSettingsWidget::updateValue(const j3x::Parameters& values)
{
    editbox_->setText(r3e::j3x::get<std::string>(values, tab_ + "/" + name_));
}

j3x::Obj StringSettingsWidget::getValue() const
{
    return std::string(editbox_->getText());
}


ControlsSettingsWidget::ControlsSettingsWidget(SettingsWindow* window, tgui::Theme* theme, const std::string& tab,
                                               const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    button_ = tgui::Button::create();
    button_->setRenderer(theme->getRenderer("ButtonLabel"));
    button_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    button_->getRenderer()->setFont(RM.getFont("default"));
    button_->setFocusable(true);

    button_->connect("mousepressed", [window, this]() {
        window->unfocusControlsWidgets();
        window->setFocusedControlsWidget(this);
    });
}

[[nodiscard]] tgui::Widget::Ptr ControlsSettingsWidget::getWidget() const
{
    return button_;
}

void ControlsSettingsWidget::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, static_cast<int>(r3e::utils::stringToKey(button_->getText())), out);
}

void ControlsSettingsWidget::updateValue(const j3x::Parameters& values)
{
    button_->setText(r3e::utils::keyToString(
            static_cast<sf::Keyboard::Key>(r3e::j3x::get<int>(values, tab_ + "/" + name_))));
}

void ControlsSettingsWidget::setFocus(bool focus)
{
    static const auto clicked_button_color = button_->getRenderer()->getTextColorDown();
    static const auto normal_button_color = button_->getRenderer()->getTextColor();
    button_->getRenderer()->setTextColor(focus ? clicked_button_color : normal_button_color);
}

void ControlsSettingsWidget::setKey(sf::Keyboard::Key key)
{
    button_->setText(r3e::utils::keyToString(key));
}

j3x::Obj ControlsSettingsWidget::getValue() const
{
    return static_cast<int>(r3e::utils::stringToKey(button_->getText()));
}


CharacterSettingsWidget::CharacterSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    grid_ = tgui::Grid::create();
    grid_->setAutoSize(true);

    pictures_container_ = tgui::Panel::create(CONF<sf::Vector2f>("graphics/face_size") * 1.1f);
    pictures_container_->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    character_ = tgui::Label::create();
    character_->setRenderer(theme->getRenderer("ItemLabel"));
    character_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    character_->getRenderer()->setFont(RM.getFont("default"));

    left_ = tgui::Button::create("<");
    left_->setRenderer(theme->getRenderer("ButtonLabel"));
    left_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    left_->getRenderer()->setFont(RM.getFont("default"));
    left_->setFocusable(true);

    right_ = tgui::Button::create(">");
    right_->setRenderer(theme->getRenderer("ButtonLabel"));
    right_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    right_->getRenderer()->setFont(RM.getFont("default"));
    right_->setFocusable(true);

    grid_->addWidget(left_, 0, 0);
    grid_->addWidget(pictures_container_, 0, 1);
    grid_->addWidget(right_, 0, 2);
    grid_->addWidget(character_, 1, 1);

    grid_->setWidgetAlignment(pictures_container_, tgui::Grid::Alignment::Up);
    grid_->setWidgetAlignment(left_, tgui::Grid::Alignment::Bottom);
    grid_->setWidgetAlignment(right_, tgui::Grid::Alignment::Bottom);
    grid_->setWidgetPadding(character_, tgui::Padding{0, 0, 0, 0.1f * CONF<sf::Vector2f>("graphics/face_size").y});
    grid_->setWidgetPadding(pictures_container_,
                            tgui::Padding{0, 0.1f * CONF<sf::Vector2f>("graphics/face_size").y, 0, 0});

    for (const auto& character : RM.getListOfObjects(CONF<std::string>("paths/characters")))
    {
        if (utils::contains(CONF<j3x::List>("forbidden_characters_to_play"), character))
            continue;

        possible_characters_.emplace_back(character);
        pictures_[character] = tgui::Picture::create(RM.getTexture("characters/" + character + "_face"));
        pictures_[character]->setSize(CONF<sf::Vector2f>("graphics/face_size"));
        pictures_[character]->setVisible(false);
        pictures_container_->add(pictures_[character]);
    }

    current_character_ = possible_characters_.begin();

    left_->connect("mousepressed", [this]() {
        this->change(--this->current_character_);
    });
    right_->connect("mousepressed", [this]() {
        this->change(++this->current_character_);
    });
}

[[nodiscard]] tgui::Grid::Alignment CharacterSettingsWidget::getAlignment() const
{
    return tgui::Grid::Alignment::Bottom;
}

[[nodiscard]] tgui::Widget::Ptr CharacterSettingsWidget::getWidget() const
{
    return grid_;
}

void CharacterSettingsWidget::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, *current_character_, out);
}

void CharacterSettingsWidget::updateValue(const j3x::Parameters& values)
{
    const auto& character = r3e::j3x::get<std::string>(values, tab_ + "/" + name_);

    auto it = std::find(possible_characters_.begin(), possible_characters_.end(), character);

    if (it == possible_characters_.end())
        throw std::runtime_error("[CharacterSettingsWidget::updateValue] Character " +
                                 character + " is forbidden!");
    change(it);
}

void CharacterSettingsWidget::change(std::vector<std::string>::iterator new_value)
{
    if (new_value < possible_characters_.begin())
        new_value = possible_characters_.end() - 1;
    if (new_value >= possible_characters_.end())
        new_value = possible_characters_.begin();

    character_->setText(utils::humanize(*new_value));

    for (auto& picture : pictures_)
        picture.second->setVisible(false);
    pictures_[*new_value]->setVisible(true);
    current_character_ = new_value;
}

j3x::Obj CharacterSettingsWidget::getValue() const
{
    return *current_character_;
}


ColorSettingsWidget::ColorSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    grid_ = tgui::Grid::create();
    grid_->setAutoSize(true);

    auto grid = tgui::Grid::create();
    grid->setAutoSize(false);
    grid->setSize(CONF<sf::Vector2f>("graphics/menu_slider_size").x,
                  CONF<sf::Vector2f>("graphics/menu_slider_size").y * 12);
    for (auto i = 0; i < 4; ++i)
    {
        sliders_[i] = tgui::Slider::create();
        sliders_[i]->setRenderer(theme->getRenderer("SliderGame"));
        sliders_[i]->setSize({CONF<sf::Vector2f>("graphics/menu_slider_size").x * 0.7f,
                              CONF<sf::Vector2f>("graphics/menu_slider_size").y});
        values_[i] = tgui::Label::create();
        values_[i]->setRenderer(theme->getRenderer("ItemLabel"));
        values_[i]->setTextSize(CONF<float>("graphics/menu_window_text_size"));
        values_[i]->getRenderer()->setFont(RM.getFont("default"));

        auto label = tgui::Label::create(COMPONENTS[i].data());
        label->setRenderer(theme->getRenderer("ItemLabel"));
        label->setTextSize(CONF<float>("graphics/menu_window_text_size"));
        label->getRenderer()->setFont(RM.getFont("default"));
        grid->addWidget(label, i, 0);
        grid->addWidget(sliders_[i], i, 1);
        grid->addWidget(values_[i], i, 2);
        grid->setWidgetPadding(label, {0, 0, 0.05f * CONF<sf::Vector2f>("graphics/menu_slider_size").x, 0});
        grid->setWidgetAlignment(label, tgui::Grid::Alignment::BottomLeft);
        grid->setWidgetAlignment(sliders_[i], tgui::Grid::Alignment::Bottom);
        grid->setWidgetAlignment(values_[i], tgui::Grid::Alignment::BottomLeft);

        sliders_[i]->setMinimum(0);
        sliders_[i]->setMaximum(255);

        sliders_[i]->connect("ValueChanged", [this, i]() {
            values_[i]->setText(utils::toString<int>(static_cast<int>(sliders_[i]->getValue())));
            updateColor();
        });
    }

    auto placeholder = tgui::Button::create();
    placeholder->setSize({CONF<sf::Vector2f>("graphics/color_widget_size").x * 2.0f, 0.0f});
    placeholder->setRenderer(theme->getRenderer("Color"));
    placeholder->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    color_ = tgui::Button::create();
    color_->setSize(CONF<sf::Vector2f>("graphics/color_widget_size"));
    color_->setRenderer(theme->getRenderer("Color"));

    grid_->addWidget(grid, 0, 0);
    grid_->addWidget(placeholder, 0, 1);
    grid_->addWidget(color_, 0, 2);
}

[[nodiscard]] tgui::Widget::Ptr ColorSettingsWidget::getWidget() const
{
    return grid_;
}

void ColorSettingsWidget::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, static_cast<int>(this->getColor().toInteger()), out);
}

void ColorSettingsWidget::updateValue(const j3x::Parameters& values)
{
    auto color = sf::Color(r3e::j3x::get<int>(values, tab_ + "/" + name_));
    sliders_[0]->setValue(color.r);
    sliders_[1]->setValue(color.g);
    sliders_[2]->setValue(color.b);
    sliders_[3]->setValue(color.a);

    for (auto i = 0; i < 4; ++i)
        values_[i]->setText(utils::toString<int>(static_cast<int>(sliders_[i]->getValue())));

    updateColor();
}

j3x::Obj ColorSettingsWidget::getValue() const
{
    return static_cast<int>(this->getColor().toInteger());
}

sf::Color ColorSettingsWidget::getColor() const
{
    return sf::Color(static_cast<sf::Uint8>(sliders_[0]->getValue()),
                     static_cast<sf::Uint8>(sliders_[1]->getValue()),
                     static_cast<sf::Uint8>(sliders_[2]->getValue()),
                     static_cast<sf::Uint8>(sliders_[3]->getValue()));
}

void ColorSettingsWidget::updateColor()
{
    auto color = this->getColor();
    color_->getRenderer()->setBackgroundColor(color);
    color_->getRenderer()->setBackgroundColorHover(color);
    color_->getRenderer()->setBackgroundColorDown(color);
}


template<class T>
SliderSettingsWidget<T>::SliderSettingsWidget(tgui::Theme* theme, const std::string& tab, const std::string& name) :
        BaseSettingsWidget(theme, tab, name)
{
    grid_ = tgui::Grid::create();
    grid_->setAutoSize(true);

    slider_ = tgui::Slider::create();
    slider_->setRenderer(theme->getRenderer("SliderGame"));
    slider_->setSize(CONF<sf::Vector2f>("graphics/menu_slider_size"));

    value_ = tgui::Label::create();
    value_->setRenderer(theme->getRenderer("ItemLabel"));
    value_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    value_->getRenderer()->setFont(RM.getFont("default"));

    grid_->addWidget(slider_, 0, 0);
    grid_->addWidget(value_, 0, 1);
    grid_->setWidgetAlignment(slider_, tgui::Grid::Alignment::Bottom);

    auto param_min_str = "ranges/" + name + "_min";
    auto param_max_str = "ranges/" + name + "_max";
    slider_->setMinimum(CONF<T>("ranges/" + name + "_min"));
    slider_->setMaximum(CONF<T>("ranges/" + name + "_max"));

    if constexpr (std::is_same<T, int>::value)
        slider_->setStep(1.0f);
    else
        slider_->setStep(0.1f);

    slider_->connect("ValueChanged", [this]() {
        value_->setText(utils::toString<T>(static_cast<T>(slider_->getValue())));
    });
}

template<class T>
[[nodiscard]] tgui::Widget::Ptr SliderSettingsWidget<T>::getWidget() const
{
    return grid_;
}

template<class T>
void SliderSettingsWidget<T>::serializeAndAppend(std::string& out) const
{
    j3x::serializeAssign(name_, static_cast<T>(slider_->getValue()), out);
}

template<class T>
void SliderSettingsWidget<T>::updateValue(const j3x::Parameters& values)
{
    slider_->setValue(r3e::j3x::get<T>(values, tab_ + "/" + name_));
    value_->setText(utils::toString<T>(static_cast<T>(slider_->getValue())));
}

template<class T>
j3x::Obj SliderSettingsWidget<T>::getValue() const
{
    return static_cast<T>(slider_->getValue());
}
