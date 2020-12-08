//
// Created by jul3x on 23.11.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H
#define RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H

#include <ui/Window.h>


class AcceptWindow : public Window {
public:
    AcceptWindow(tgui::Gui *gui, tgui::Theme *theme, const std::string& text,
                 const sf::Vector2f& pos, const sf::Vector2f& size);

    void bindFunction(const std::function<void()>& yes);

protected:
    tgui::Button::Ptr yes_, no_;
};

#endif //RAG3_GAME_INCLUDE_UI_ACCEPTWINDOW_H