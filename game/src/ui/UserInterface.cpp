//
// Created by jul3x on 27.02.19.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>

#include <ui/UserInterface.h>
#include <Game.h>


UserInterface::UserInterface() :
        blood_splash_(sf::Vector2f(CFG.getInt("graphics/window_width_px"), CFG.getInt("graphics/window_height_px"))),
        weapons_bar_({static_cast<float>(CFG.getInt("graphics/window_width_px")),
                      static_cast<float>(CFG.getInt("graphics/window_height_px"))}),
        health_bar_({CFG.getInt("graphics/window_width_px") - HEALTH_BAR_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                     CFG.getInt("graphics/window_height_px") - HEALTH_BAR_Y_ * CFG.getFloat("graphics/user_interface_zoom")}),
        time_bar_({TIME_BAR_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                   CFG.getInt("graphics/window_height_px") - TIME_BAR_Y_ * CFG.getFloat("graphics/user_interface_zoom")}),
        logo_(sf::Vector2f{CFG.getInt("graphics/window_width_px") - LOGO_OFF_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                           LOGO_OFF_Y_ * CFG.getFloat("graphics/user_interface_zoom")},
              CFG.getFloat("graphics/user_interface_zoom") * sf::Vector2f{LOGO_SIZE_X_, LOGO_SIZE_Y_},
              &RM.getTexture("rag3_logo")),
        fps_text_("FPS: ", RM.getFont(), 30),
        object_use_text_("[F] Use object", RM.getFont(), 24 * CFG.getFloat("graphics/user_interface_zoom")),
        left_hud_({0.0f, static_cast<float>(CFG.getInt("graphics/window_height_px"))}),
        right_hud_({static_cast<float>(CFG.getInt("graphics/window_width_px")),
                    static_cast<float>(CFG.getInt("graphics/window_height_px"))}),
        player_(nullptr),
        camera_(nullptr) {}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[UserInterface] player_ or camera_ is nullptr!");
    }
    health_bar_.setMaxHealth(player_->getMaxHealth());
    time_bar_.setMaxTime(CFG.getFloat("journal_max_time"));

    fps_text_.setFillColor(sf::Color::White);
    fps_text_.setPosition(FPS_X_, FPS_Y_);

    object_use_text_.setFillColor(sf::Color::White);
    auto object_use_text_rect = object_use_text_.getLocalBounds();
    object_use_text_.setOrigin(object_use_text_rect.left + object_use_text_rect.width/2.0f,
                               object_use_text_rect.top  + object_use_text_rect.height/2.0f);

    graphics.getWindow().setMouseCursorVisible(false);
    graphics.getWindow().setKeyRepeatEnabled(false);

    camera_->setViewNormalSize(graphics.getWindow().getView().getSize());
}

void UserInterface::registerPlayer(Player* player)
{
    player_ = player;
}

void UserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void UserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    updatePlayerStates();
    handleMouse(graphics.getWindow());
    handleKeys();

    blood_splash_.update(time_elapsed);

//    std::stringstream fps_stream;
//    fps_stream << std::fixed << std::setprecision(2) << Game::get().getFPS();
//    fps_text_.setString("FPS: " + fps_stream.str());

    auto special_object = Game::get().getCurrentSpecialObject();
    if (special_object != nullptr)
    {
        object_use_text_.setPosition(special_object->getPosition() - sf::Vector2f{0.0f, OBJECT_USE_TEXT_OFFSET_Y_});
        object_use_text_.setFillColor(sf::Color::White);
    }
    else
    {
        object_use_text_.setFillColor(sf::Color::Transparent);
    }

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

                health_bar_.setPosition(event.size.width - HEALTH_BAR_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                                        event.size.height - HEALTH_BAR_Y_ * CFG.getFloat("graphics/user_interface_zoom"));

                time_bar_.setPosition(TIME_BAR_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                                        event.size.height - TIME_BAR_Y_ * CFG.getFloat("graphics/user_interface_zoom"));

                weapons_bar_.setPosition(event.size.width, event.size.height);
                logo_.setPosition(event.size.width - LOGO_OFF_X_ * CFG.getFloat("graphics/user_interface_zoom"),
                                  LOGO_OFF_Y_ * CFG.getFloat("graphics/user_interface_zoom"));

                left_hud_.setPosition(0.0f, event.size.height);
                right_hud_.setPosition(event.size.width, event.size.height);

                camera_->setViewNormalSize(visible_area);
                blood_splash_.resizeWindow(visible_area);

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    auto current_view = graphics.getCurrentView();
                    current_view.zoom(1.0f - (event.mouseWheelScroll.delta > 0 ? 0.1f : -0.1f));
                    graphics.modifyCurrentView(current_view);
                    camera_->setViewNormalSize(graphics.getCurrentView().getSize());
                }
                else
                {
                    handleScrolling(event.mouseWheelScroll.delta);
                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                if (event.key.code == sf::Keyboard::F)
                {
                    Game::get().useSpecialObject();
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    if (Game::get().getGameState() == Game::GameState::Paused)
                        Game::get().setGameState(Game::GameState::Normal);
                    else
                        Game::get().setGameState(Game::GameState::Paused);
                }
                else if (event.key.code == sf::Keyboard::R && !Game::get().isJournalFreezed())
                {
                    Game::get().setGameState(Game::GameState::Reverse);
                }
                break;
            }
            case sf::Event::KeyReleased:
            {
                if (event.key.code == sf::Keyboard::R)
                {
                    Game::get().setGameState(Game::GameState::Normal);
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

void UserInterface::draw(graphics::Graphics& graphics)
{
    graphics.setCurrentView();
    graphics.getWindow().draw(object_use_text_);
    graphics.setStaticView();

    graphics.draw(blood_splash_);

    //graphics.draw(fps_text_);
    graphics.draw(logo_);
    graphics.draw(left_hud_);
    graphics.draw(right_hud_);
    graphics.draw(health_bar_);
    graphics.draw(time_bar_);
    graphics.draw(weapons_bar_);
    graphics.draw(crosshair_);
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
        delta.x -= utils::getFloat(RM.getObjectParams("characters", "player"), "max_speed");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        delta.x += utils::getFloat(RM.getObjectParams("characters", "player"), "max_speed");
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        delta.y -= utils::getFloat(RM.getObjectParams("characters", "player"), "max_speed");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        delta.y += utils::getFloat(RM.getObjectParams("characters", "player"), "max_speed");
    }

    if (player_->isAlive())
        player_->setVelocity(delta.x, delta.y);
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);

    if (player_->isAlive() && Game::get().getGameState() == Game::GameState::Normal)
    {
        player_->setWeaponPointing(mouse_world_pos);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player_->shot(Game::get().getCurrentTimeFactor()))
        {
            camera_->setShaking();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            camera_->setPointingTo(player_->getPosition() +
                                   utils::geo::getNormalized(mouse_world_pos - player_->getPosition()) *
                                   CFG.getFloat("graphics/camera_right_click_distance_factor"));
            camera_->setZoomTo(CFG.getFloat("graphics/camera_right_click_zoom_factor"));
            Game::get().setBulletTime();
        }
        else
        {
            camera_->setPointingTo(player_->getPosition());
            camera_->setZoomTo(1.0f);
            Game::get().setNormalTime();
        }
    }
}

inline void UserInterface::updatePlayerStates()
{
    weapons_bar_.updateWeaponsList(player_->getWeapons(), player_->getCurrentWeapon());

    health_bar_.updateHealth(player_->getHealth());

    time_bar_.updateTime(Game::get().getJournal().getDurationSaved());
    time_bar_.setFreeze(Game::get().isJournalFreezed() && Game::get().getGameState() != Game::GameState::Reverse);

    blood_splash_.updateLifeState(player_->getLifeState());
}
