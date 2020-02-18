//
// Created by jul3x on 27.02.19.
//

#include <engine/system/Engine.h>
#include <engine/utils/Geometry.h>

#include <game/ui/UserInterface.h>


UserInterface::UserInterface() :
        weapons_bar_({CFG.getInt("window_width_px") / 2.0f, CFG.getInt("window_height_px") -
                                                            WEAPONS_BAR_OFF_Y_ * CFG.getFloat("user_interface_zoom")}),
        health_bar_({HEALTH_BAR_X_ * CFG.getFloat("user_interface_zoom"),
                     HEALTH_BAR_Y_ * CFG.getFloat("user_interface_zoom")}),
        player_(nullptr),
        camera_(nullptr) {}

void UserInterface::initialize()
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[UserInterface] player_ or camera_ is nullptr!");
    }
    health_bar_.setMaxHealth(player_->getMaxHealth());
}

void UserInterface::registerPlayer(Player* player)
{
    player_ = player;
}

void UserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void UserInterface::handleEvents(Graphics& graphics)
{
    static sf::Event event;

    updatePlayerStates();
    handleMouse(graphics.getWindow());
    handleKeys();

    while (graphics.getWindow().pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
            {
                graphics.getWindow().close();
                break;
            }
            case sf::Event::Resized:
            {
                auto visible_area = sf::Vector2f(event.size.width, event.size.height);

                auto current_view = graphics.getCurrentView();
                current_view.setSize(visible_area);
                graphics.modifyCurrentView(current_view);

                auto static_view = graphics.getStaticView();
                static_view.setSize(visible_area);
                static_view.setCenter(visible_area / 2.0f);
                graphics.modifyStaticView(static_view);

                weapons_bar_.setPosition(event.size.width / 2.0f,
                                         event.size.height - WEAPONS_BAR_OFF_Y_ * CFG.getFloat("user_interface_zoom"));

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    auto current_view = graphics.getCurrentView();
                    current_view.zoom(1.0f - (event.mouseWheelScroll.delta > 0 ? 0.05f : -0.05f));
                    graphics.modifyCurrentView(current_view);
                }
                else
                {
                    handleScrolling(event.mouseWheelScroll.delta);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void UserInterface::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(weapons_bar_, states);
    target.draw(health_bar_, states);
}

inline void UserInterface::handleScrolling(float delta)
{
    auto do_increase = delta > 0 ? 1 : -1;

    if (player_->isAlive())
        player_->switchWeapon(do_increase);
}

inline void UserInterface::handleKeys()
{
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

    if (player_->isAlive())
        player_->setVelocity(delta.x, delta.y);
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);

    if (player_->isAlive())
        player_->setWeaponPointing(graphics_window.mapPixelToCoords(mouse_pos));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (player_->isAlive() && player_->shot())
        {
            camera_->setShaking();
        }
    }
}

inline void UserInterface::updatePlayerStates()
{
    weapons_bar_.updateWeaponsList(player_->getWeapons());
    weapons_bar_.updateCurrentWeapon(player_->getCurrentWeapon());

    health_bar_.updateHealth(player_->getHealth());
}
