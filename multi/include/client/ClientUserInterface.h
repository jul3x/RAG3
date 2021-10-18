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

};


#endif //RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
