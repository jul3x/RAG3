//
// Created by jul3x on 08.05.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_WEAPONWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_WEAPONWINDOW_H

#include <common/PlacedWeapon.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class WeaponWindow : public ChildWindow {
    public:
        WeaponWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setObjectContent(const std::string& category, PlacedWeapon* weapon);

    private:
        PlacedWeapon* weapon_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr button_;

        tgui::EditBox::Ptr id_box_;
        tgui::TextBox::Ptr fun_box_, data_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_WEAPONWINDOW_H