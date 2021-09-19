//
// Created by jul3x on 15.08.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_SETTINGSWINDOW_H
#define RAG3_GAME_INCLUDE_UI_MENU_SETTINGSWINDOW_H

#include <TGUI/TGUI.hpp>

#include <R3E/j3x/J3X.h>

#include <ui/menu/MenuWindow.h>


using namespace r3e;

class Framework;

class BaseSettingsWidget;

class ControlsSettingsWidget;

class SettingsWindow : public MenuWindow {
public:
    explicit SettingsWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework);

    void doShow(bool show) override;

    void setKey(sf::Keyboard::Key k);
    void unfocusControlsWidgets();
    void setFocusedControlsWidget(ControlsSettingsWidget* widget);

private:
    void createWidgets();
    void updateValues();
    void saveValues();

    void onTabSelected(tgui::Gui& gui, const std::string& selected_tab)
    {
        for (const auto& tab : tab_names_)
            gui.get(tab)->setVisible(false);

        gui.get(selected_tab)->setVisible(true);
        scroll_panel_->setVerticalScrollbarValue(0);
        scroll_panel_->setContentSize(gui.get(selected_tab)->getSize());
        unfocusControlsWidgets();
    }

    tgui::ScrollablePanel::Ptr scroll_panel_;
    tgui::Tabs::Ptr tabs_;

    tgui::Button::Ptr save_button_, restore_button_;

    std::vector<std::string> tab_names_;
    std::unordered_map<std::string, std::unique_ptr<BaseSettingsWidget>> widgets_;
    ControlsSettingsWidget* focused_controls_widget_;

    std::unordered_map<std::string, std::shared_ptr<j3x::J3XVisitor>> params_;
    Framework* framework_;
    tgui::Theme* theme_;
    tgui::Gui* gui_;

};


/* Widgets */

class BaseSettingsWidget {
public:
    BaseSettingsWidget(tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Label::Ptr getLabel() const;
    [[nodiscard]] virtual tgui::Grid::Alignment getAlignment() const;
    [[nodiscard]] virtual tgui::Widget::Ptr getWidget() const = 0;
    virtual void serializeAndAppend(std::string& out) const = 0;
    virtual void updateValue(const j3x::Parameters& values) = 0;

protected:
    std::string name_;

private:
    tgui::Label::Ptr label_;

};


class BoolSettingsWidget : public BaseSettingsWidget {
public:
    BoolSettingsWidget(tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Widget::Ptr getWidget() const override;
    void serializeAndAppend(std::string& out) const override;
    void updateValue(const j3x::Parameters& values) override;

private:
    tgui::CheckBox::Ptr checkbox_;

};


class StringSettingsWidget : public BaseSettingsWidget {
public:
    StringSettingsWidget(tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Widget::Ptr getWidget() const override;
    void serializeAndAppend(std::string& out) const override;
    void updateValue(const j3x::Parameters& values) override;

private:
    tgui::EditBox::Ptr editbox_;

};


class ControlsSettingsWidget : public BaseSettingsWidget {
public:
    ControlsSettingsWidget(SettingsWindow* window, tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Widget::Ptr getWidget() const override;
    void serializeAndAppend(std::string& out) const override;
    void updateValue(const j3x::Parameters& values) override;
    void setFocus(bool focus);
    void setKey(sf::Keyboard::Key key);
private:
    tgui::Button::Ptr button_;
};


class CharacterSettingsWidget : public BaseSettingsWidget {
public:
    CharacterSettingsWidget(tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Grid::Alignment getAlignment() const override;
    [[nodiscard]] tgui::Widget::Ptr getWidget() const override;
    void serializeAndAppend(std::string& out) const override;
    void updateValue(const j3x::Parameters& values) override;

private:
    void change(std::vector<std::string>::iterator new_value);

    std::vector<std::string> possible_characters_;
    std::vector<std::string>::iterator current_character_;

    tgui::Grid::Ptr grid_;
    tgui::Button::Ptr left_, right_;
    tgui::Label::Ptr character_;
    tgui::Panel::Ptr pictures_container_;
    std::unordered_map<std::string, tgui::Picture::Ptr> pictures_;
};


template<class T>
class SliderSettingsWidget : public BaseSettingsWidget {
public:
    SliderSettingsWidget(tgui::Theme* theme, const std::string& name);
    [[nodiscard]] tgui::Widget::Ptr getWidget() const override;
    void serializeAndAppend(std::string& out) const override;
    void updateValue(const j3x::Parameters& values) override;

protected:
    tgui::Grid::Ptr grid_;
    tgui::Slider::Ptr slider_;
    tgui::Label::Ptr value_;
};


#endif //RAG3_GAME_INCLUDE_UI_MENU_SETTINGSWINDOW_H
