//
// Created by jul3x on 16.03.21.
//

#include <iomanip>

#include <R3E/system/Engine.h>

#include <common/ResourceManager.h>

#include <client/ClientUserInterface.h>
#include <client/Client.h>


ClientUserInterface::ClientUserInterface(Client* client) :
        UserInterface(client)
{
}

void ClientUserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[ClientUserInterface] player_ or camera_ is nullptr!");
    }

    UserInterface::initialize(graphics);
    time_bar_.setFreeze(true);
    small_backpack_hud_.doShow(true);
    menu_->doShow(false);
}
