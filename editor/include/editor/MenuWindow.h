//
// Created by jul3x on 16.03.20.
//

#ifndef RAG3_EDITOR_MENUWINDOW_H
#define RAG3_EDITOR_MENUWINDOW_H

#include <editor/ChildWindow.h>


namespace editor {
    class UserInterface;

    class MenuWindow : public ChildWindow {
    public:
        MenuWindow(UserInterface* ui, tgui::Gui *gui, tgui::Theme *theme);

    private:
        UserInterface* ui_;
        tgui::Grid::Ptr grid_;


    };

} // namespace editor

#endif //RAG3_EDITOR_MENUWINDOW_H