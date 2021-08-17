//
// Created by macie on 05/08/2021.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_PARAMETERSWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_PARAMETERSWINDOW_H

#include <R3E/j3x/J3X.h>

#include <common/Map.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class ParametersWindow : public ChildWindow {
    public:
        ParametersWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setParameters();

    private:

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr button_;

        tgui::EditBox::Ptr shader_box_, background_box_, lighting_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_PARAMETERSWINDOW_H
