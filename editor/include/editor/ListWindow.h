//
// Created by jul3x on 14.03.20.
//

#ifndef RAG3_EDITOR_LISTWINDOW_H
#define RAG3_EDITOR_LISTWINDOW_H

#include <TGUI/TGUI.hpp>

namespace editor {

    class ListWindow {
    public:
        ListWindow(tgui::Gui *gui, tgui::Theme *theme, std::string title, const sf::Vector2f& pos);

        void initialize(const std::vector<std::string> &tabs, const std::vector<std::string> &paths_to_objects);

    private:
        void onTabSelected(tgui::Gui& gui, std::string selectedTab)
        {
            for (const auto &tab : tab_names_)
                gui.get(tab)->setVisible(false);

            gui.get(selectedTab)->setVisible(true);
        }

        std::string title_;

        tgui::Gui *gui_;
        tgui::Theme *theme_;

        tgui::ChildWindow::Ptr child_;
        std::vector<tgui::Grid::Ptr> grids_;

        tgui::Tabs::Ptr tabs_;
        std::vector<std::vector<tgui::Picture::Ptr>> clickables_;
        std::vector<std::string> tab_names_;

        sf::Vector2f pos_;
    };

} // namespace editor

#endif //RAG3_EDITOR_LISTWINDOW_H
