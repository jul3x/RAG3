//
// Created by jul3x on 20.07.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_ABOUTWINDOW_H
#define RAG3_GAME_INCLUDE_UI_MENU_ABOUTWINDOW_H

#include <TGUI/TGUI.hpp>

#include <ui/menu/MenuWindow.h>


using namespace r3e;

class AboutWindow : public MenuWindow {
public:
    explicit AboutWindow(tgui::Gui* gui, tgui::Theme* theme);

    void doShow(bool show) override;

private:
    tgui::Label::Ptr text_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_ABOUTWINDOW_H
