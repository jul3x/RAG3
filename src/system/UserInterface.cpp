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
            default:
            {
                break;
            }
        }
    }
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
}
