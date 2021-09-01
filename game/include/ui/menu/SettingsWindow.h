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

class SettingsWindow : public MenuWindow {
public:
    explicit SettingsWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework);

    void doShow(bool show) override;

    void setKey(sf::Keyboard::Key k);

private:
    tgui::Widget::Ptr createWidget(const j3x::Parameters& values,
                                   const std::string& param, const std::string& type, int i);
    tgui::Widget::Ptr createControlsWidget(const j3x::Parameters& values, const std::string& param);
    tgui::Widget::Ptr createGeneralWidget(const j3x::Parameters& values, const std::string& param,
                                          const std::string& type, int i);
    void createWidgets();
    void updateValues();
    void saveValues();
    void unfocusControlsWidgets();

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

    std::vector<tgui::Grid::Ptr> grids_;
    std::vector<std::string> tab_names_;
    std::vector<tgui::Button*> controls_widgets_;
    tgui::Button* focused_controls_widget_;

    std::unordered_map<std::string, std::shared_ptr<j3x::J3XVisitor>> params_;
    Framework* framework_;
    tgui::Theme* theme_;
    tgui::Gui* gui_;

    sf::Color clicked_button_color_, normal_button_color_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_SETTINGSWINDOW_H
