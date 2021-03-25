//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_UNIQUEOBJECTWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_UNIQUEOBJECTWINDOW_H

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class UniqueObjectWindow : public ChildWindow {
    public:
        UniqueObjectWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setObjectContent(const std::string& category, const std::string& name, int id);

    private:
        tgui::Grid::Ptr grid_;

        tgui::EditBox::Ptr text_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_UNIQUEOBJECTWINDOW_H
