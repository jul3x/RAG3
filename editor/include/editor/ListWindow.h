//
// Created by jul3x on 14.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_LISTWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_LISTWINDOW_H

#include <unordered_map>

#include <TGUI/TGUI.hpp>

#include <editor/ChildWindow.h>


namespace editor {

    class UserInterface;

    class ListWindow : public ChildWindow {
    public:
        ListWindow(UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme, std::string title, const sf::Vector2f& pos,
                   std::string id = "");

        void initialize(const std::vector<std::string>& tabs, const std::vector<std::string>& paths_to_objects);

    private:
        void onTabSelected(tgui::Gui& gui, const std::string& selected_tab)
        {
            for (const auto& tab : tab_names_)
                gui.get(tab)->setVisible(false);

            gui.get(selected_tab)->setVisible(true);
        }

        UserInterface* ui_;

        std::vector<tgui::Grid::Ptr> grids_;

        std::unordered_map<std::string, tgui::Texture> button_textures_;

        tgui::Tabs::Ptr tabs_;
        std::vector<std::vector<tgui::Picture::Ptr>> clickables_;
        std::vector<std::string> tab_names_;
    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_LISTWINDOW_H
