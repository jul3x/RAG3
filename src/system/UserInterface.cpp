//
// Created by jprolejko on 27.02.19.
//

#include <system/Engine.h>
#include <utils/Geometry.h>

#include <system/UserInterface.h>


UserInterface::UserInterface() : 
    weapons_bar_({CFG.getInt("window_width_px") / 2.0f, CFG.getInt("window_height_px") - 70.0f}) {}

void UserInterface::handleEvents() {
    static sf::Event event;
    static auto &graphics_window = Graphics::getInstance().getWindow();
    static auto& player = Engine::getInstance().getPlayer();

    updatePlayerStates(player);
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

void UserInterface::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(weapons_bar_, states);
}

inline void UserInterface::handleScrolling(Player &player, float delta) {
    auto do_increase = delta > 0 ? 1 : -1;

    player.switchWeapon(do_increase);
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
        if (player.shot())
        {
            Engine::getInstance().forceCameraShaking();
        }
    }
}

inline void UserInterface::updatePlayerStates(const Player &player) {
    weapons_bar_.updateWeaponsList(player.getWeapons());
    weapons_bar_.updateCurrentWeapon(player.getCurrentWeapon());
}
