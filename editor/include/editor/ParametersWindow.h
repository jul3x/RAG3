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

        const std::vector<std::string> map_parameters = {"background_color", "lighting_color", "shader"};

        std::unordered_map<std::string, tgui::EditBox::Ptr> parameters_boxes_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_PARAMETERSWINDOW_H
