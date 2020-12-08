//
// Created by jul3x on 08.12.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H
#define RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H

#include <ui/Window.h>


class NoteWindow : public Window {
public:
    NoteWindow(tgui::Gui *gui, tgui::Theme *theme, const std::string& text,
                 const sf::Vector2f& pos, const sf::Vector2f& size);

protected:
    tgui::Button::Ptr ok_;

};

#endif //RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H