//
// Created by jul3x on 16.03.20.
//

#ifndef RAG3_EDITOR_LOADWINDOW_H
#define RAG3_EDITOR_LOADWINDOW_H

#include <editor/ChildWindow.h>


namespace editor {

    class LoadWindow : public ChildWindow {
    public:
        LoadWindow(tgui::Gui *gui, tgui::Theme *theme);

        void refreshMapList(const std::vector<std::string>& map_list);
    private:
        tgui::Grid::Ptr grid_;
        tgui::ListBox::Ptr list_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_LOADWINDOW_H
