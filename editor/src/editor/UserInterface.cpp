//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>

#include <editor/ResourceManager.h>
#include <editor/UserInterface.h>
#include <Editor.h>

using namespace editor;

UserInterface::UserInterface() :
        logo_(sf::Vector2f{CFG.getInt("window_width_px") - LOGO_OFF_X_ * CFG.getFloat("user_interface_zoom"),
                           LOGO_OFF_Y_ * CFG.getFloat("user_interface_zoom")},
              CFG.getFloat("user_interface_zoom") * sf::Vector2f{LOGO_SIZE_X_, LOGO_SIZE_Y_},
              &RM.getTexture("rag3_logo")),
        gui_theme_("../data/config/gui_theme.txt"),
        tiles_window_(&gui_, &gui_theme_, "Environment", {CFG.getFloat("tiles_window_x"), CFG.getFloat("tiles_window_y")}) {}

void UserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    gui_.setTarget(graphics.getWindow());
    tiles_window_.initialize({"obstacles_tiles", "decorations_tiles"}, {CFG.getString("paths/obstacles_tiles"), CFG.getString("paths/decorations_tiles")});
}

void UserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    handleMouse(graphics.getWindow());
    handleKeys();

    while (graphics.getWindow().pollEvent(event))
    {
        if (gui_.handleEvent(event))
            continue;
        
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

                camera_->setViewNormalSize(current_view.getSize());
                gui_.setView(static_view);

                logo_.setPosition(event.size.width - LOGO_OFF_X_ * CFG.getFloat("user_interface_zoom"),
                                  LOGO_OFF_Y_ * CFG.getFloat("user_interface_zoom"));

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                auto current_view = graphics.getCurrentView();
                current_view.zoom(1.0f - (event.mouseWheelScroll.delta > 0 ? 0.1f : -0.1f));
                graphics.modifyCurrentView(current_view);

                camera_->setViewNormalSize(current_view.getSize());

                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    Editor::get().placeItem(crosshair_.getPosition());
                }
                else
                {
                    Editor::get().removeItem(crosshair_.getPosition());
                }
            }
            default:
            {
                break;
            }
        }
    }
}

void UserInterface::draw(graphics::Graphics& graphics)
{
    graphics.setCurrentView();
    graphics.draw(crosshair_);
    graphics.setStaticView();
    graphics.draw(logo_);
    gui_.draw();
}

inline void UserInterface::handleKeys()
{
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    crosshair_.setPosition(mouse_world_pos);

    const auto& current_item = Editor::get().getCurrentItem();

    if (current_item.second.empty())
    {
        crosshair_.setColor(0, 0, 0, 0);
    }
    else
    {
        crosshair_.setColor(255, 255, 255, 120);
        crosshair_.changeTexture(&RM.getTexture(current_item.first + "/" + current_item.second), true);

        if (current_item.first == "obstacles_tiles")
        {
            crosshair_.setSize({ObstacleTile::SIZE_X_, ObstacleTile::SIZE_Y_});
            crosshair_.changeOrigin({ObstacleTile::SIZE_X_ / 2.0f, ObstacleTile::SIZE_Y_ / 2.0f + ObstacleTile::OFFSET_Y_});
        }
        else
        {
            crosshair_.setSize({DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_});
            crosshair_.changeOrigin({DecorationTile::SIZE_X_ / 2.0f, DecorationTile::SIZE_Y_ / 2.0f});
        }
    }
}

