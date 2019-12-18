//
// Created by jprolejko on 27.02.19.
//

#include <engine/system/Engine.h>
#include <engine/utils/Geometry.h>

#include <game/ui/UserInterface.h>


UserInterface::UserInterface() : 
    weapons_bar_({CFG.getInt("window_width_px") / 2.0f, CFG.getInt("window_height_px") -
                                                        WEAPONS_BAR_OFF_Y_ * CFG.getFloat("user_interface_zoom")}),
    health_bar_({HEALTH_BAR_X_ * CFG.getFloat("user_interface_zoom"),
                 HEALTH_BAR_Y_ * CFG.getFloat("user_interface_zoom")}) {}

void UserInterface::initialize() {
    health_bar_.setMaxHealth(Engine::getInstance().getPlayer().getMaxHealth());
}

void UserInterface::handleEvents() {
    static sf::Event event;
    static auto &graphics_window = Graphics::getInstance().getWindow();
    static auto &player = Engine::getInstance().getPlayer();

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

                auto current_view = Graphics::getInstance().getCurrentView();
                current_view.setSize(visible_area);
                Graphics::getInstance().modifyCurrentView(current_view);

                auto static_view = Graphics::getInstance().getStaticView();
                static_view.setSize(visible_area);
                static_view.setCenter(visible_area / 2.0f);
                Graphics::getInstance().modifyStaticView(static_view);

                weapons_bar_.setPosition(event.size.width / 2.0f,
                                         event.size.height - WEAPONS_BAR_OFF_Y_ * CFG.getFloat("user_interface_zoom"));

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
    target.draw(health_bar_, states);
}

inline void UserInterface::handleScrolling(Player &player, float delta) {
    auto do_increase = delta > 0 ? 1 : -1;

    if (player.isAlive())
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

    if (player.isAlive())
        player.setVelocity(delta.x, delta.y);
}

inline void UserInterface::handleMouse(sf::RenderWindow &graphics_window, Player &player) {
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_difference = graphics_window.mapPixelToCoords(mouse_pos) -
        player.getPosition();

    float angle;
    std::tie(std::ignore, angle) = utils::cartesianToPolar(mouse_difference);

    if (player.isAlive())
        player.setRotation(angle * 180.0f / M_PI);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (player.isAlive() && player.shot())
        {
            Engine::getInstance().forceCameraShaking();
        }
    }
}

inline void UserInterface::updatePlayerStates(const Player &player) {
    weapons_bar_.updateWeaponsList(player.getWeapons());
    weapons_bar_.updateCurrentWeapon(player.getCurrentWeapon());
    
    health_bar_.updateHealth(player.getHealth());
}
