//
// Created by jul3x on 27.02.19.
//

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>

#include <editor/ResourceManager.h>
#include <editor/UserInterface.h>

using namespace editor;

UserInterface::UserInterface() :
        logo_(sf::Vector2f{CFG.getInt("window_width_px") - LOGO_OFF_X_ * CFG.getFloat("user_interface_zoom"),
                           LOGO_OFF_Y_ * CFG.getFloat("user_interface_zoom")},
              CFG.getFloat("user_interface_zoom") * sf::Vector2f{LOGO_SIZE_X_, LOGO_SIZE_Y_},
              &RM.getTexture("rag3_logo")) {}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    graphics.getWindow().setMouseCursorVisible(false);
}

void UserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

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

                logo_.setPosition(event.size.width - LOGO_OFF_X_ * CFG.getFloat("user_interface_zoom"),
                                  LOGO_OFF_Y_ * CFG.getFloat("user_interface_zoom"));

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    auto current_view = graphics.getCurrentView();
                    current_view.zoom(1.0f - (event.mouseWheelScroll.delta > 0 ? 0.1f : -0.1f));
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
    target.draw(logo_, states);
    target.draw(crosshair_, states);
}

inline void UserInterface::handleScrolling(float delta)
{
    auto do_increase = delta > 0 ? 1 : -1;
}

inline void UserInterface::handleKeys()
{
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);
}

