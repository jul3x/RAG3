//
// Created by jul3x on 16.03.21.
//

#include <client/ClientFullHud.h>
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
    full_hud_ = std::make_unique<ClientFullHud>(this, client_,
                                                sf::Vector2f{
                                                        static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                        static_cast<float>(CONF<int>("graphics/window_height_px"))});
    // Blah...
    client_->setMultiStats(dynamic_cast<ClientFullHud*>(full_hud_.get())->getStats());
    time_bar_.setFreeze(true);
    menu_->doShow(true);

//    debug_info_.setFont(RM.getFont());
//    debug_info_.setCharacterSize(36);
//    debug_info_.setPosition(100, 200);
}

void ClientUserInterface::openMenu()
{
    UserInterface::openMenu();
    client_->disconnect();
}

void ClientUserInterface::spawnThought(Character* user, const std::string& text)
{
}

void ClientUserInterface::spawnNoteWindow(const std::string& text, bool note_info)
{
    if (!note_info)
        UserInterface::spawnNoteWindow(text, note_info);
}

void ClientUserInterface::update(graphics::Graphics& graphics, float time_elapsed)
{
    UserInterface::update(graphics, time_elapsed);

//    std::string new_debug_info =
//            "Current status: " + utils::toString(static_cast<int>(client_->getConnectionStatus())) +
//            " Other players:\n";
//
//    for (const auto& conn : client_->getPlayers())
//    {
//        const auto* player = conn.second.player.get();
//        new_debug_info += sf::IpAddress(conn.first).toString() + ": " + player->getName() + ", health: " +
//                          utils::toString(player->getHealth()) + "/" +
//                          utils::toString(player->getMaxHealth()) + "\n";
//    }
//    debug_info_.setString(new_debug_info);
}

void ClientUserInterface::draw(graphics::Graphics& graphics)
{
    UserInterface::draw(graphics);
//    graphics.draw(debug_info_);
}

std::string ClientUserInterface::generateMessage(MessageType type, const j3x::Parameters& params)
{
    DeathCause death_cause;
    switch (type)
    {
        case MessageType::Death:
            death_cause = static_cast<DeathCause>(j3x::get<int>(params, "cause"));
            switch (death_cause)
            {
                case DeathCause::Drown:
                    return j3x::get<std::string>(params, "name") + " cannot swim :(.";
                case DeathCause::Fire:
                    return j3x::get<std::string>(params, "name") + " smells like fried chicken.";
                case DeathCause::Explosion:
                    return j3x::get<std::string>(params, "name") + " broke into pieces.";
                case DeathCause::OtherCharacterBullet:
                    return j3x::get<std::string>(params, "killer") + " shot " + j3x::get<std::string>(params, "name") +
                           ".";
                case DeathCause::OtherCharacterMelee:
                    return j3x::get<std::string>(params, "killer") + " cut " + j3x::get<std::string>(params, "name") +
                           ".";
                default:
                    return j3x::get<std::string>(params, "name") + " died with unknown reason.";
            }
        case MessageType::Respawn:
            return j3x::get<std::string>(params, "name") + " returned from the ashes.";
        case MessageType::Left:
            return j3x::get<std::string>(params, "name") + " cannot do this anymore.";
        case MessageType::Connection:
            return j3x::get<std::string>(params, "name") + " comes to life!";
        case MessageType::Talk:
            return j3x::get<std::string>(params, "name") + " says: " + j3x::get<std::string>(params, "msg");
        default:
            return "Unknown message";
    }
}
