//
// Created by jul3x on 16.03.21.
//

#include <client/ClientUserInterface.h>
#include <client/Client.h>


ClientUserInterface::ClientUserInterface(Client* client) :
        UserInterface(client),
        client_(client)
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
            {"Connect to server", [this]() { menu_->showWindow(Menu::Window::PickServer); }},
            {"Settings",          [this]() { menu_->showWindow(Menu::Window::Settings); }},
            {"About",             [this]() { menu_->showWindow(Menu::Window::About); }},
            {"Exit",              [this]() { framework_->close(); }}
    });
    full_hud_ = std::make_unique<ExtendedFullHud>(this, framework_,
                                                  sf::Vector2f{
                                                          static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                          static_cast<float>(CONF<int>("graphics/window_height_px"))});
    dynamic_cast<ExtendedFullHud*>(full_hud_.get())->bindRespawn([this]() {
        this->client_->respawnWithoutReload();
        this->clearWindows();
        this->full_hud_->show(false);
    });

    time_bar_.setFreeze(true);
    menu_->doShow(true);
}

void ClientUserInterface::openMenu()
{
    UserInterface::openMenu();
    client_->disconnect();
}
