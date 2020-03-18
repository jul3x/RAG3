//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>

#include <editor/ResourceManager.h>
#include <editor/UserInterface.h>
#include <editor/Character.h>
#include <editor/Collectible.h>

#include <Editor.h>


using namespace editor;

UserInterface::UserInterface() :
        logo_(sf::Vector2f{CFG.getInt("window_width_px") - LOGO_OFF_X_ * CFG.getFloat("user_interface_zoom"),
                           LOGO_OFF_Y_ * CFG.getFloat("user_interface_zoom")},
              CFG.getFloat("user_interface_zoom") * sf::Vector2f{LOGO_SIZE_X_, LOGO_SIZE_Y_},
              &RM.getTexture("rag3_logo")),
        gui_theme_("../data/config/gui_theme.txt"),
        tiles_window_(this, &gui_, &gui_theme_, "Tiles",
                      {CFG.getFloat("tiles_window_x"), CFG.getFloat("tiles_window_y")}, "tiles_window"),
        objects_window_(this, &gui_, &gui_theme_, "Objects",
                        {CFG.getFloat("objects_window_x"), CFG.getFloat("objects_window_y")}, "objects_window"),
        menu_window_(this, &gui_, &gui_theme_),
        save_window_(&gui_, &gui_theme_),
        load_window_(&gui_, &gui_theme_),
        config_window_(&gui_, &gui_theme_)
{
    gui_.get("save_window")->setVisible(false);
    gui_.get("load_window")->setVisible(false);
    gui_.get("config_window")->setVisible(false);

    information_.setPosition(CFG.getFloat("info_x"), CFG.getFloat("info_y"));
    information_.setFillColor(sf::Color(255, 255, 255, 0));
    information_.setFont(RM.getFont("editor"));
    information_.setString("");
    information_.setCharacterSize(20);

    gui_.setFont(RM.getFont("editor"));
}

void UserInterface::generateMenuBar(sf::RenderWindow& window)
{
    auto list_menu = tgui::MenuBar::create();
    list_menu->setRenderer(gui_theme_.getRenderer("MenuBar"));
    list_menu->setSize("100%", 35);
    list_menu->addMenu("File");
    list_menu->addMenuItem("File", "Clear existing map");
    list_menu->connectMenuItem("File", "Clear existing map", [&]() { Editor::get().clearMap(); });

    list_menu->addMenuItem("File", "Load map");
    list_menu->connectMenuItem("File", "Load map", [this]() { this->gui_.get("load_window")->setVisible(true); });

    list_menu->addMenuItem("File", "Save map");
    list_menu->connectMenuItem("File", "Save map", [this]() { this->gui_.get("save_window")->setVisible(true); });

    list_menu->addMenuItem("File", "Exit");
    list_menu->connectMenuItem("File", "Exit", [&window]() { window.close(); });

    list_menu->addMenu("Windows");
    list_menu->addMenuItem("Windows", "Menu");
    list_menu->connectMenuItem("Windows", "Menu", [this]() { this->gui_.get("menu_window")->setVisible(true); });

    list_menu->addMenuItem("Windows", "Tiles");
    list_menu->connectMenuItem("Windows", "Tiles", [this]() { this->gui_.get("tiles_window")->setVisible(true); });

    list_menu->addMenuItem("Windows", "Objects");
    list_menu->connectMenuItem("Windows", "Objects", [this]() { this->gui_.get("objects_window")->setVisible(true); });

    gui_.add(list_menu);
}

void UserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    gui_.setTarget(graphics.getWindow());
    tiles_window_.initialize({"obstacles_tiles", "decorations_tiles"}, {CFG.getString("paths/obstacles_tiles"), CFG.getString("paths/decorations_tiles")});
    objects_window_.initialize({"characters", "collectibles"}, {CFG.getString("paths/characters"), CFG.getString("paths/collectibles")});

    generateMenuBar(graphics.getWindow());
}

void UserInterface::resetMapList()
{
    load_window_.refreshMapList(RM.getFreshListOfObjects(CFG.getString("paths/maps_dir")));
    save_window_.refreshMapName(Editor::get().getCurrentMapName());
}

void UserInterface::openConfigWindow(const std::string& category, const std::string& id)
{
    config_window_.setConfigContent(category, id);
    gui_.get("config_window")->setVisible(true);
}

void UserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    handleMouse(graphics.getWindow());
    handleKeys();

    information_.setFillColor({information_.getFillColor().r, information_.getFillColor().g,
                               information_.getFillColor().b,
                               static_cast<sf::Uint8>(information_.getFillColor().a - time_elapsed * CFG.getFloat("info_fade_speed"))});

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
    graphics.getWindow().draw(information_);
    gui_.draw();
}

void UserInterface::spawnInfo(const std::string& msg)
{
    information_.setString(msg);
    information_.setFillColor(sf::Color::White);
}

void UserInterface::spawnError(const std::string& msg)
{
    information_.setString(msg);
    information_.setFillColor(sf::Color::Red);
}

inline void UserInterface::handleKeys()
{
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    handleCameraCenter(graphics_window, mouse_world_pos);
    handleCrosshair(graphics_window, mouse_world_pos);

    bool is_on_widget = false;

    for (auto& widget : gui_.getWidgets())
        if (widget->mouseOnWidget({static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y)}) &&
            widget->isVisible())
             is_on_widget = true;

    if (!is_on_widget && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Editor::get().placeItem(crosshair_.getPosition());
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            Editor::get().removeItem(crosshair_.getPosition());
        }
    }
}

inline void UserInterface::handleCameraCenter(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        camera_->setPointingTo(camera_->getPointingTo() +
                               CFG.getFloat("view_move_speed") *
                               utils::geo::vectorLengthLimit(previous_mouse_world_pos_ - mouse_world_pos, 1.0f));
    }

    previous_mouse_world_pos_ = mouse_world_pos;
}

inline void UserInterface::handleCrosshair(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos)
{
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
        crosshair_.setSize(
                sf::Vector2f(utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "size_x"),
                             utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "size_y")));
        crosshair_.changeOrigin(
                sf::Vector2f(utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "size_x"),
                             utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "size_y")) / 2.0f +
                        sf::Vector2f(utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "map_offset_x"),
                                     utils::getFloat(RM.getObjectParams(current_item.first, current_item.second), "map_offset_y")));
    }
}
