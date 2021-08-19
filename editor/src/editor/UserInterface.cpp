//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>

#include <common/characters/Character.h>
#include <common/ResourceManager.h>

#include <editor/UserInterface.h>

#include <Editor.h>


using namespace editor;

UserInterface::UserInterface() :
        logo_(sf::Vector2f{CONF<int>("window_width_px") - CONF<sf::Vector2f>("logo_pos_off").x,
                           CONF<sf::Vector2f>("logo_pos_off").y},
              CONF<sf::Vector2f>("logo_size"),
              &RM.getTexture("rag3_logo")),
        gui_theme_("../data/config/gui_theme.txt"),
        tiles_window_(this, &gui_, &gui_theme_, "Tiles",
                      CONF<sf::Vector2f>("tiles_window_pos"), "tiles_window"),
        objects_window_(this, &gui_, &gui_theme_, "Objects",
                        CONF<sf::Vector2f>("objects_window_pos"), "objects_window"),
        menu_window_(this, &gui_, &gui_theme_),
        save_window_(&gui_, &gui_theme_),
        load_window_(&gui_, &gui_theme_),
        config_window_(&gui_, &gui_theme_),
        unique_object_window_(&gui_, &gui_theme_),
        special_object_window_(&gui_, &gui_theme_),
        character_object_window_(&gui_, &gui_theme_),
        weapon_window_(&gui_, &gui_theme_),
        obstacle_object_window_(&gui_, &gui_theme_),
        marked_item_(nullptr)
{
    gui_.get("save_window")->setVisible(false);
    gui_.get("load_window")->setVisible(false);
    gui_.get("config_window")->setVisible(false);
    gui_.get("unique_object_window")->setVisible(false);
    gui_.get("special_object_window")->setVisible(false);
    gui_.get("weapon_window")->setVisible(false);
    gui_.get("character_object_window")->setVisible(false);
    gui_.get("obstacle_object_window")->setVisible(false);

    information_.setPosition(CONF<sf::Vector2f>("info_pos"));
    information_.setFillColor(sf::Color(255, 255, 255, 0));
    information_.setFont(RM.getFont("editor"));
    information_.setString("");
    information_.setCharacterSize(CONF<float>("information_text_size"));
    information_a_ = 0.0f;

    gui_.setFont(RM.getFont("editor"));
}

void UserInterface::generateMenuBar(sf::RenderWindow& window)
{
    auto list_menu = tgui::MenuBar::create();
    list_menu->setRenderer(gui_theme_.getRenderer("MenuBar"));
    list_menu->setSize("100%", CONF<float>("menu_bar_height"));
    list_menu->setTextSize(CONF<float>("label_text_size"));
    list_menu->addMenu("File");
    list_menu->addMenuItem("File", "Clear existing map");
    list_menu->connectMenuItem("File", "Clear existing map", [&]() { Editor::get().clearMap(); });

    list_menu->addMenuItem("File", "Load map");
    list_menu->connectMenuItem("File", "Load map", [this]() {
        this->resetMapList();
        this->gui_.get("load_window")->setVisible(true);
    });

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
    tiles_window_.initialize({"obstacles_tiles", "decorations_tiles"},
                             {CONF<std::string>("paths/obstacles_tiles"),
                              CONF<std::string>("paths/decorations_tiles")});
    objects_window_.initialize({"characters", "specials", "obstacles", "decorations", "weapons"},
                               {CONF<std::string>("paths/characters"), CONF<std::string>("paths/specials"),
                                CONF<std::string>("paths/obstacles"), CONF<std::string>("paths/decorations"),
                                CONF<std::string>("paths/weapons")});


    generateMenuBar(graphics.getWindow());
}

void UserInterface::resetMapList()
{
    load_window_.refreshMapList(RM.getFreshListOfObjects(CONF<std::string>("paths/maps_dir")));
    save_window_.refreshMapName(Editor::get().getCurrentMapName());
}

void UserInterface::openConfigWindow(const std::string& category, const std::string& id)
{
    config_window_.setConfigContent(category, id);
    gui_.get("config_window")->setVisible(true);
}

void UserInterface::openUniqueObjectWindow(const std::string& category, const std::string& name, int id)
{
    unique_object_window_.setObjectContent(category, name, id);
    gui_.get("unique_object_window")->setVisible(true);
}

void UserInterface::openSpecialObjectWindow(const std::string& category, Special* obj)
{
    special_object_window_.setObjectContent(category, obj);
    gui_.get("special_object_window")->setVisible(true);
}

void UserInterface::openCharacterObjectWindow(const std::string& category, Character* obj)
{
    character_object_window_.setObjectContent(category, obj);
    gui_.get("character_object_window")->setVisible(true);
}

void UserInterface::openWeaponWindow(const std::string& category, PlacedWeapon* obj)
{
    weapon_window_.setObjectContent(category, obj);
    gui_.get("weapon_window")->setVisible(true);
}

void UserInterface::openObstacleWindow(const std::string& category, Obstacle* obj)
{
    obstacle_object_window_.setObjectContent(category, obj);
    gui_.get("obstacle_object_window")->setVisible(true);
}

void UserInterface::setZIndex(int value)
{
    max_z_index_ = value;
}

int UserInterface::getZIndex() const
{
    return max_z_index_;
}

void UserInterface::update(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    handleMouse(graphics.getWindow(), time_elapsed);
    handleKeys();

    information_a_ = std::max(0.0f, information_a_ - time_elapsed * CONF<float>("info_fade_speed"));
    information_.setFillColor({information_.getFillColor().r, information_.getFillColor().g,
                               information_.getFillColor().b, static_cast<sf::Uint8>(information_a_)});

    Editor::get().markCurrentItem(crosshair_.getPosition());

    while (graphics.getWindow().pollEvent(event))
    {
        gui_.handleEvent(event);

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

                logo_.setPosition(event.size.width - CONF<sf::Vector2f>("logo_pos_off").x,
                                  CONF<sf::Vector2f>("logo_pos_off").y);

                Editor::get().resetLighting(visible_area);

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (mouse_on_widget_)
                    break;

                auto current_view = graphics.getCurrentView();
                current_view.zoom(1.0f - (event.mouseWheelScroll.delta > 0 ? 0.1f : -0.1f));
                graphics.modifyCurrentView(current_view);

                camera_->setViewNormalSize(current_view.getSize());

                break;
            }
            case sf::Event::KeyPressed:
            {
                if (event.key.code == sf::Keyboard::E)
                {
                    if (!mouse_on_widget_)
                        Editor::get().readItemInfo(crosshair_.getPosition());
                }
                else if (event.key.code == sf::Keyboard::F1)
                {
                    if (special_object_window_.isDataFocused())
                    {
                        special_object_window_.addToData(crosshair_.getPositionStr() + ";");
                    }
                    else if (obstacle_object_window_.isDataFocused())
                    {
                        obstacle_object_window_.addToData(crosshair_.getPositionStr() + ";");
                    }
                }
                else if (event.key.code == sf::Keyboard::F2)
                {
                    auto id = Editor::get().readItemInfo(crosshair_.getPosition(), true);

                    if (id != -1)
                    {
                        if (special_object_window_.isDataFocused())
                        {
                            special_object_window_.addToData(std::to_string(id) + ";");
                        }
                        else if (obstacle_object_window_.isDataFocused())
                        {
                            obstacle_object_window_.addToData(std::to_string(id) + ";");
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    gui_.get("save_window")->setVisible(false);
                    gui_.get("load_window")->setVisible(false);
                    gui_.get("config_window")->setVisible(false);
                    gui_.get("unique_object_window")->setVisible(false);
                    gui_.get("special_object_window")->setVisible(false);
                    gui_.get("weapon_window")->setVisible(false);
                    gui_.get("character_object_window")->setVisible(false);
                    gui_.get("obstacle_object_window")->setVisible(false);
                }
                else if (event.key.code == sf::Keyboard::Q)
                {
                    Editor::get().setCurrentItem("", "");
                }
            }
            case sf::Event::MouseButtonPressed:
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    previous_mouse_world_pos_ = crosshair_.getPosition();
                }
                else
                {
                    auto mouse_pos = sf::Mouse::getPosition(graphics.getWindow());
                    auto mouse_world_pos = graphics.getWindow().mapPixelToCoords(mouse_pos);
                    previous_mouse_world_pos_ = mouse_world_pos;
                }

                if (!mouse_on_widget_)
                {
                    if (event.mouseButton.button == sf::Mouse::Left &&
                        !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                    {
                        if (Editor::get().getCurrentItem().first != "weapons")
                            Editor::get().placeItem(crosshair_.getPosition());
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right &&
                             !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                    {
                        Editor::get().removeItem(crosshair_.getPosition());
                    }
                    else if (event.mouseButton.button == sf::Mouse::Middle)
                    {
                        marked_item_ = Editor::get().getMarkedItem();
                    }
                }

                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if (event.mouseButton.button == sf::Mouse::Middle)
                {
                    marked_item_ = nullptr;
                }
                else if (event.mouseButton.button == sf::Mouse::Left &&
                         !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && !mouse_on_widget_)
                {
                    if (Editor::get().getCurrentItem().first == "weapons")
                        Editor::get().placeItem(previous_mouse_world_pos_,
                                                180.0f / M_PI * std::get<1>(utils::geo::cartesianToPolar(
                                                        crosshair_.getPosition() - previous_mouse_world_pos_)));
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
    graphics.getWindow().draw(information_);
    gui_.draw();
    RM.setFontsSmoothAllowed(false);
}

void UserInterface::spawnInfo(const std::string& msg)
{
    information_.setString(msg);
    information_.setFillColor(sf::Color::White);
    information_a_ = 255.0f;
}

void UserInterface::spawnError(const std::string& msg)
{
    information_.setString(msg);
    information_.setFillColor(sf::Color::Red);
    information_a_ = 255.0f;
}

inline void UserInterface::handleKeys()
{
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window, float time_elapsed)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    handleCameraCenter(graphics_window, mouse_world_pos, time_elapsed);
    handleCrosshair(graphics_window, mouse_world_pos, time_elapsed);

    mouse_on_widget_ = false;

    for (auto& widget : gui_.getWidgets())
        if (widget->mouseOnWidget({static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y)}) &&
            widget->isVisible())
            mouse_on_widget_ = true;

    if (!mouse_on_widget_ && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
        Editor::get().getCurrentItem().first.find("tile") != std::string::npos)
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

    if (marked_item_ != nullptr)
    {
        marked_item_->setPosition(crosshair_.getPosition());
    }
}

inline void UserInterface::handleCameraCenter(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos,
                                              float time_elapsed)
{
    static sf::Vector2f previous_mouse_pos = mouse_world_pos;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        mouse_camera_center_vectors_.emplace_back(previous_mouse_pos - mouse_world_pos);

        if (mouse_camera_center_vectors_.size() > MIN_VECTORS_TO_MOVE_)
        {
            sf::Vector2f vec = {};
            for (const auto& vector : mouse_camera_center_vectors_)
            {
                vec += vector;
            }

            vec = vec / static_cast<float>(mouse_camera_center_vectors_.size()) *
                  CFG.get<float>("camera_moving_factor");
            camera_->setPointingTo(camera_->getPointingTo() + vec);

            mouse_camera_center_vectors_.pop_front();
        }
    }
    else
    {
        mouse_camera_center_vectors_.clear();
    }

    previous_mouse_pos = mouse_world_pos;
}

inline void UserInterface::handleCrosshair(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos,
                                           float time_elapsed)
{
    const auto& current_item = Editor::get().getCurrentItem();
    auto is_tile = false;

    if (current_item.first.find("tile") != std::string::npos)
    {
        crosshair_.setPosition(DecorationTile::SIZE_X_ * std::round(mouse_world_pos.x / DecorationTile::SIZE_X_),
                               DecorationTile::SIZE_Y_ * std::round(mouse_world_pos.y / DecorationTile::SIZE_Y_));
        is_tile = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        crosshair_.setPosition(
                DecorationTile::SIZE_X_ / 4.0f * std::round(mouse_world_pos.x / DecorationTile::SIZE_X_ * 4.0f),
                DecorationTile::SIZE_Y_ / 4.0f * std::round(mouse_world_pos.y / DecorationTile::SIZE_Y_ * 4.0f));
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
    {
        crosshair_.setPosition(DecorationTile::SIZE_X_ * std::round(mouse_world_pos.x / DecorationTile::SIZE_X_),
                               DecorationTile::SIZE_Y_ * std::round(mouse_world_pos.y / DecorationTile::SIZE_Y_));
    }
    else
    {
        crosshair_.setPosition(mouse_world_pos);
    }

    if (current_item.second.empty())
    {
        crosshair_.setColor(0, 0, 0, 0);
    }
    else
    {
        crosshair_.setColor(255, 255, 255, 120);

        auto crosshair_texture_name = !RMHAS<bool>(current_item.first, current_item.second, "is_drawable") ||
                                      RMGET<bool>(current_item.first, current_item.second, "is_drawable", false) ?
                                      current_item.first + "/" + current_item.second : "specials/special_event";
        crosshair_.changeTexture(&RM.getTexture(crosshair_texture_name), true);

        if (is_tile)
        {
            crosshair_.setSize({DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_});
            crosshair_.changeOrigin({DecorationTile::SIZE_X_ / 2.0f, DecorationTile::SIZE_Y_ / 2.0f});
        }
        else
        {
            crosshair_.setSize(RMGET<sf::Vector2f>(current_item.first, current_item.second, "size"));
            crosshair_.changeOrigin(RMGET<sf::Vector2f>(current_item.first, current_item.second, "size") / 2.0f
                                    + RMGET<sf::Vector2f>(current_item.first, current_item.second, "map_offset"));

            if (RMGET<int>(current_item.first, current_item.second, "frames_number") > 1)
                crosshair_.changeTextureRect({{0, 0},
                                              static_cast<sf::Vector2i>(RMGET<sf::Vector2f>(current_item.first,
                                                                                            current_item.second,
                                                                                            "size"))});

        }

        if (current_item.first == "weapons" && sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            crosshair_.setRotation(180.0f / M_PI * std::get<1>(utils::geo::cartesianToPolar(crosshair_.getPosition() -
                                                                                            previous_mouse_world_pos_)));
            crosshair_.setPosition(previous_mouse_world_pos_);
        }
        else
        {
            crosshair_.setRotation(0.0f);
        }
    }
}

