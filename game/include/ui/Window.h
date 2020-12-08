//
// Created by jul3x on 08.12.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_WINDOW_H
#define RAG3_GAME_INCLUDE_UI_WINDOW_H

#include <TGUI/TGUI.hpp>


class Window {
public:
    Window(tgui::Gui *gui, tgui::Theme *theme, const sf::Vector2f& pos, const sf::Vector2f& size);

    ~Window();

protected:
    tgui::Gui *gui_;
    tgui::Theme *theme_;

    tgui::ChildWindow::Ptr child_;
};

#endif //RAG3_GAME_INCLUDE_UI_WINDOW_H