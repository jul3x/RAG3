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

    health_bar_.setMaxHealth(player_->getMaxHealth());
}

void ClientUserInterface::draw(graphics::Graphics& graphics)
{
    graphics.setCurrentView();
    graphics.getWindow().draw(object_use_text_);

    graphics.setStaticView();

    graphics.draw(right_hud_);
    graphics.draw(health_bar_);
    graphics.draw(weapons_bar_);
    graphics.draw(small_backpack_hud_);

    gui_->draw();
    graphics.draw(crosshair_);

    RM.setFontsSmoothAllowed(false);
}

void ClientUserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);

    bool is_gui = false;
    is_left_mouse_pressed_ = false;

    for (const auto& widget : gui_->getWidgets())
    {
        if (widget->mouseOnWidget({static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y)}) && widget->isVisible())
        {
            is_gui = true;
            break;
        }
    }

    if (player_->isAlive())
    {
        player_->setWeaponPointing(mouse_world_pos);

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            is_left_mouse_pressed_ = true;
            if (player_->shot())
                camera_->setShaking();
        }

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            camera_->setPointingTo(player_->getPosition() +
                                   utils::geo::getNormalized(mouse_world_pos - player_->getPosition()) *
                                   CONF<float>("graphics/camera_right_click_distance_factor"));
            camera_->setZoomTo(CONF<float>("graphics/camera_right_click_zoom_factor"));
        }
        else
        {
            camera_->setPointingTo(player_->getPosition());
            camera_->setZoomTo(1.0f);
        }
    }
}
