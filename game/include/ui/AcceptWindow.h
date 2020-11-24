//
// Created by jul3x on 23.11.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H
#define RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H

#include <TGUI/TGUI.hpp>


class AcceptWindow {
public:
    AcceptWindow(tgui::Gui *gui, tgui::Theme *theme, const std::string& text,
                 const sf::Vector2f& pos, const sf::Vector2f& size);

    void bindFunction(const std::function<void()>& yes);

    ~AcceptWindow();

protected:
    tgui::Gui *gui_;
    tgui::Theme *theme_;

    tgui::ChildWindow::Ptr child_;
    tgui::Button::Ptr yes_, no_;

    sf::Vector2f pos_;
};

#endif //RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H