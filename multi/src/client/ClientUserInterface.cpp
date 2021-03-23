//
// Created by jul3x on 16.03.21.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>

#include <client/ClientUserInterface.h>
#include <client/Client.h>


ClientUserInterface::ClientUserInterface() :
        object_use_text_("[F] Use object", RM.getFont(), CONF<float>("graphics/use_text_size")),
        player_(nullptr),
        camera_(nullptr),
        theme_("../data/config/gui_theme.txt") {}

void ClientUserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[ClientUserInterface] player_ or camera_ is nullptr!");
    }
    object_use_text_.setFillColor(sf::Color::White);

    graphics.getWindow().setMouseCursorVisible(false);
    graphics.getWindow().setKeyRepeatEnabled(false);
    graphics.getCurrentView().zoom(1.0f / CONF<float>("graphics/global_zoom"));
    graphics.setCurrentView();
    camera_->setViewNormalSize(graphics.getWindow().getView().getSize());

    gui_ = std::make_unique<tgui::Gui>(graphics.getWindow());

    tgui::ToolTip::setInitialDelay({});
    tgui::ToolTip::setDistanceToMouse({-tgui::ToolTip::getDistanceToMouse().x, tgui::ToolTip::getDistanceToMouse().y});
}

void ClientUserInterface::registerPlayer(Player* player)
{
    player_ = player;
}

void ClientUserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void ClientUserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    updatePlayerStates(time_elapsed);
    handleMouse(graphics.getWindow());
    handleKeys();

    // TODO
//    auto special_object = Game::get().getCurrentSpecialObject();
//    if (special_object != nullptr)
//    {
//        object_use_text_.setString(special_object->getTextToUse());
//        auto object_use_text_rect = object_use_text_.getLocalBounds();
//        object_use_text_.setOrigin(object_use_text_rect.left + object_use_text_rect.width/2.0f,
//                                   object_use_text_rect.top  + object_use_text_rect.height/2.0f);
//
//        object_use_text_.setPosition(special_object->getPosition() - sf::Vector2f{0.0f, OBJECT_USE_TEXT_OFFSET_Y_});
//        object_use_text_.setFillColor(sf::Color::White);
//    }
//    else
//    {
//        object_use_text_.setFillColor(sf::Color::Transparent);
//    }

    while (graphics.getWindow().pollEvent(event))
    {
        gui_->handleEvent(event);

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
                current_view.zoom(1.0f / CONF<float>("graphics/global_zoom"));
                graphics.modifyCurrentView(current_view);

                auto static_view = graphics.getStaticView();
                static_view.setSize(visible_area);
                static_view.setCenter(visible_area / 2.0f);
                graphics.modifyStaticView(static_view);

                camera_->setViewNormalSize(graphics.getWindow().getView().getSize());

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                handleScrolling(event.mouseWheelScroll.delta);
                break;
            }
            case sf::Event::KeyPressed:
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Q:
                    {
                        Client::get().getPlayer().sideStep(Player::SideStepDir::Left);
                        break;
                    }
                    case sf::Keyboard::E:
                    {
                        Client::get().getPlayer().sideStep(Player::SideStepDir::Right);
                        break;
                    }
                    case sf::Keyboard::F:
                    {
                        Client::get().useSpecialObject();
                        break;
                    }
                    case sf::Keyboard::Escape:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
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

void ClientUserInterface::draw(graphics::Graphics& graphics)
{
    graphics.setCurrentView();
    graphics.getWindow().draw(object_use_text_);
    graphics.setStaticView();

    gui_->draw();
    graphics.draw(crosshair_);

    RM.setFontsSmoothAllowed(false);
}

inline void ClientUserInterface::handleScrolling(float delta)
{
    auto do_increase = delta > 0 ? 1 : -1;

    if (player_->isAlive())
        player_->switchWeapon(do_increase);
}

inline void ClientUserInterface::handleKeys()
{
    keys_pressed_.clear();
    auto delta = sf::Vector2f(0.0f, 0.0f);

    float max_speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        keys_pressed_.insert(sf::Keyboard::LShift);
        max_speed = RMGET<float>("characters", "player", "max_running_speed");
    }
    else
        max_speed = RMGET<float>("characters", "player", "max_speed");


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        delta.x -= max_speed;
        keys_pressed_.insert(sf::Keyboard::A);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        delta.x += max_speed;
        keys_pressed_.insert(sf::Keyboard::D);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        delta.y -= max_speed;
        keys_pressed_.insert(sf::Keyboard::W);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        delta.y += max_speed;
        keys_pressed_.insert(sf::Keyboard::S);
    }

    if (player_->isAlive())
        player_->setVelocity(sf::Vector2f{delta.x, delta.y} * player_->getSpeedFactor());
}

inline void ClientUserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);

    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);

    bool is_gui = false;

    for (const auto& widget : gui_->getWidgets())
    {
        if (widget->mouseOnWidget({static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y)}) && widget->isVisible())
        {
            is_gui = true;
            break;
        }
    }

    if (player_->isAlive())
    {
        player_->setWeaponPointing(mouse_world_pos);

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Left) && player_->shot())
        {
            camera_->setShaking();
        }

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            camera_->setPointingTo(player_->getPosition() +
                                   utils::geo::getNormalized(mouse_world_pos - player_->getPosition()) *
                                   CONF<float>("graphics/camera_right_click_distance_factor"));
            camera_->setZoomTo(CONF<float>("graphics/camera_right_click_zoom_factor"));
        }
        else
        {
            camera_->setPointingTo(player_->getPosition());
            camera_->setZoomTo(1.0f);
        }
    }
}

inline void ClientUserInterface::updatePlayerStates(float time_elapsed)
{
}

const std::set<sf::Keyboard::Key>& ClientUserInterface::getKeysPressed()
{
    return keys_pressed_;
}

bool ClientUserInterface::isLeftMousePressed() const
{
    return sf::Mouse::isButtonPressed(sf::Mouse::Left);
}
