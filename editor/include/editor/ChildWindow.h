//
// Created by jul3x on 16.03.20.
//

#ifndef RAG3_EDITOR_CHILDWINDOW_H
#define RAG3_EDITOR_CHILDWINDOW_H

#include <TGUI/TGUI.hpp>


using namespace r3e;

namespace editor {

    class ChildWindow {
    public:
        ChildWindow(tgui::Gui *gui,
                    tgui::Theme *theme,
                    std::string title,
                    const sf::Vector2f& pos,
                    const sf::Vector2f& size,
                    std::string id = "");

    protected:
        std::string title_;

        tgui::Gui *gui_;
        tgui::Theme *theme_;

        tgui::ChildWindow::Ptr child_;

        sf::Vector2f pos_;
    };

} // namespace editor

#endif //RAG3_EDITOR_CHILDWINDOW_H