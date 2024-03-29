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

#include <packets/PlayersStatePacket.h>

#include <client/TalkBox.h>


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

    void spawnNoteWindow(const std::string& text, bool note_info = true) override;

    static std::string generateMessage(MessageType type, const j3x::Parameters& params);

private:
    void updatePlayerStates(float time_elapsed) override;
    void setTalking(bool is_talking) override;
    void enterTalking() override;

    Client* client_;

    std::unique_ptr<TalkBox> talk_box_;
};


#endif //RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
