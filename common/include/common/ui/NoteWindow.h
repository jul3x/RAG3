//
// Created by jul3x on 08.12.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H
#define RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H

#include <common/ui/Window.h>


class Framework;

class NoteWindow : public Window {
public:
    NoteWindow(Framework* framework, UserInterface* ui, const std::string& text,
               const sf::Vector2f& pos, const sf::Vector2f& size);

protected:
    Framework* framework_;
    tgui::Button::Ptr ok_;

};

#endif //RAG3_GAME_INCLUDE_UI_NOTEWINDOW_H