//
// Created by jul3x on 20.07.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_LOADGAMEWINDOW_H
#define RAG3_GAME_INCLUDE_UI_MENU_LOADGAMEWINDOW_H

#include <TGUI/TGUI.hpp>

#include <common/ui/menu/MenuWindow.h>


using namespace r3e;

class Framework;

class LoadGameWindow : public MenuWindow {
public:
    explicit LoadGameWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework);

    void doShow(bool show) override;

private:
    tgui::Label::Ptr text_;
    tgui::ScrollablePanel::Ptr scroll_panel_;

    Framework* framework_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_LOADGAMEWINDOW_H
