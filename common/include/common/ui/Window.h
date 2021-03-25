//
// Created by jul3x on 08.12.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_WINDOW_H
#define RAG3_GAME_INCLUDE_UI_WINDOW_H

#include <TGUI/TGUI.hpp>


class UserInterface;

class Window {
public:
    Window(UserInterface* ui, const sf::Vector2f& pos, const sf::Vector2f& size);

    ~Window();

protected:
    UserInterface* ui_;
    tgui::Gui* gui_;
    tgui::Theme* theme_;

    tgui::ChildWindow::Ptr child_;
};

#endif //RAG3_GAME_INCLUDE_UI_WINDOW_H