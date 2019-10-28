//
// Created by jprolejko on 27.02.19.
//

#include <system/Engine.h>
#include <utils/Geometry.h>

#include <system/UserInterface.h>


UserInterface::UserInterface() {}

void UserInterface::handleEvents() {
    static sf::Event event;
    static auto &graphics_window = Graphics::getInstance().getWindow();
    static auto& player = Engine::getInstance().getPlayer();

    handleMouse(graphics_window, player);
    handleKeys(player);

    while (graphics_window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
            {
                graphics_window.close();
                break;
            }
            case sf::Event::Resized:
            {
                auto visible_area = sf::Vector2f(event.size.width, event.size.height);

                auto view = Graphics::getInstance().getView();
                view.setSize(visible_area);
                Graphics::getInstance().setView(view);

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                handleScrolling(player, event.mouseWheelScroll.delta);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

inline void UserInterface::handleScrolling(Player &player, float delta) {
    static const int WEAPONS_NUMBER = 4;
    auto do_increase = delta > 0 ? 1 : -1;

    player.switchWeapon(do_increase);
    /*
    current_weapon_number_ = (current_weapon_number_ + do_increase) % WEAPONS_NUMBER;

    switch (current_weapon_number_)
    {
        case 0:
        {
            player.setWeapon("m4");
            break;
        }
        case 1:
        {
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
        default:
        {
            throw std::runtime_error("Dude! Current weapon is not supposed to be different "
                                     "than all possible weapons!");
        }
    }*/
}

inline void UserInterface::handleKeys(Player &player) {
    auto delta = sf::Vector2f(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        delta.x -= CFG.getFloat("player_max_speed");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        delta.x += CFG.getFloat("player_max_speed");
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        delta.y -= CFG.getFloat("player_max_speed");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        delta.y += CFG.getFloat("player_max_speed");
    }
  //  auto yaw = player.getRotation() * M_PI / 180.0f + M_PI_2;

   /// player.setVelocity(delta.x * std::cos(yaw) - delta.y * std::sin(yaw),
  //                     delta.x * std::sin(yaw) + delta.y * std::cos(yaw));
    player.setVelocity(delta.x, delta.y);
}

inline void UserInterface::handleMouse(sf::RenderWindow &graphics_window, Player &player) {
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_difference = graphics_window.mapPixelToCoords(mouse_pos) -
        player.getPosition();

    float angle;
    std::tie(std::ignore, angle) = utils::cartesianToPolar(mouse_difference);
    player.setRotation(angle * 180.0f / M_PI);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        player.shot();
    }
}
