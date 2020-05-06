//
// Created by jul3x on 17.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_CONFIGWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_CONFIGWINDOW_H

#include <editor/ChildWindow.h>

using namespace r3e;

namespace editor {

    class ConfigWindow : public ChildWindow {
    public:
        ConfigWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setConfigContent(const std::string& category, const std::string& id);

    private:
        tgui::Grid::Ptr grid_;

        tgui::TextBox::Ptr text_box_;

        tgui::Button::Ptr button_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_CONFIGWINDOW_H
