//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_GAME_INCLUDE_UI_GAMEUSERINTERFACE_H
#define RAG3_GAME_INCLUDE_UI_GAMEUSERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>

#include <common/ui/UserInterface.h>


using namespace r3e;

class Game;

class GameUserInterface : public UserInterface {

public:
    explicit GameUserInterface(Game* game);

    GameUserInterface(const GameUserInterface&) = delete;

    GameUserInterface& operator=(const GameUserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;

    void spawnNoteWindow(const std::string& text, bool note_info = true) override;

private:
    void updatePlayerStates(float time_elapsed) override;

    bool canZoomIn(bool is_mouse_on_widget) const override;
    bool canZoomOut() const override;
    void zoomOut() override;

    Game* game_;

};


#endif //RAG3_GAME_INCLUDE_UI_GAMEUSERINTERFACE_H
