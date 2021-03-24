//
// Created by jul3x on 09.05.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_CHARACTEROBJECTWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_CHARACTEROBJECTWINDOW_H

#include <common/characters/Character.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class CharacterObjectWindow : public ChildWindow {
    public:
        CharacterObjectWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setObjectContent(const std::string& category, Character* character);

        [[nodiscard]] bool isDataFocused() const;

        void addToData(const std::string& str);

    private:
        Character* character_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr button_;

        tgui::EditBox::Ptr id_box_, act_box_;
        tgui::TextBox::Ptr fun_box_, data_box_, talk_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_CHARACTEROBJECTWINDOW_H