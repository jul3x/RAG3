//
// Created by jul3x on 16.03.21.
//

#ifndef RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
#define RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <common/ui/hud/WeaponsBar.h>
#include <common/ui/hud/RightHud.h>
#include <common/ui/Crosshair.h>
#include <common/ui/Achievement.h>
#include <common/ui/AcceptWindow.h>
#include <common/ui/Window.h>
#include <common/misc/Thought.h>
#include <common/misc/BonusText.h>
#include <common/characters/Player.h>
#include <common/misc/Camera.h>
#include <common/ui/UserInterface.h>


using namespace r3e;

class Client;

class ClientUserInterface : public UserInterface {
public:
    explicit ClientUserInterface(Client* client);
    ClientUserInterface(const ClientUserInterface&) = delete;
    ClientUserInterface& operator=(const ClientUserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;
    void update(graphics::Graphics& graphics, float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;
    void openMenu() override;

    void spawnThought(Character* user, const std::string& text) override;
    void spawnNoteWindow(const std::string& text, bool note_info = true) override;

private:
    Client* client_;

    sf::Text debug_info_;

};


#endif //RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
