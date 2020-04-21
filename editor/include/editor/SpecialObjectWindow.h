//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_EDITOR_SPECIALOBJECTWINDOW_H
#define RAG3_EDITOR_SPECIALOBJECTWINDOW_H

#include <common/Special.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class SpecialObjectWindow : public ChildWindow {
    public:
        SpecialObjectWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setObjectContent(const std::string& category, Functional* special);

    private:
        Functional* functional_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr button_;

        tgui::EditBox::Ptr id_box_, act_box_;
        tgui::TextBox::Ptr fun_box_, data_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_SPECIALOBJECTWINDOW_H