//
// Created by jul3x on 17.10.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_PICKSERVERWINDOW_H
#define RAG3_GAME_INCLUDE_UI_MENU_PICKSERVERWINDOW_H

#include <TGUI/TGUI.hpp>

#include <common/ui/menu/MenuWindow.h>


using namespace r3e;

class Framework;

class PickServerWindow : public MenuWindow {
public:
    explicit PickServerWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework);

    void doShow(bool show) override;

private:
    tgui::Label::Ptr text_;
    tgui::EditBox::Ptr editbox_;
    tgui::Button::Ptr button_;

    Framework* framework_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_PICKSERVERWINDOW_H
