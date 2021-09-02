//
// Created by macie on 05/08/2021.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_CONTROLSWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_CONTROLSWINDOW_H

#include <R3E/j3x/J3X.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class ControlsWindow : public ChildWindow {
    public:
        ControlsWindow(tgui::Gui* gui, tgui::Theme* theme);

    private:
        tgui::Grid::Ptr grid_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_CONTROLSWINDOW_H
