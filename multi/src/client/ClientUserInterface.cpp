//
// Created by jul3x on 16.03.21.
//

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
    menu_->makeMenuElements({
            {"Connect to server", [this]() { framework_->startGame(); }},
            {"Settings",          [this]() { menu_->showWindow(Menu::Window::Settings); }},
            {"About",             [this]() { menu_->showWindow(Menu::Window::About); }},
            {"Exit",              [this]() { framework_->close(); }}
    });
    full_hud_ = std::make_unique<FullHud>(this, framework_,
                                          sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                       static_cast<float>(CONF<int>("graphics/window_height_px"))});
    time_bar_.setFreeze(true);
    menu_->doShow(true);
}
