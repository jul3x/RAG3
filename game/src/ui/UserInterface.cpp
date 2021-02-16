//
// Created by jul3x on 27.02.19.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>

#include <ui/UserInterface.h>
#include <ui/NoteWindow.h>
#include <Game.h>


UserInterface::UserInterface() :
        blood_splash_(sf::Vector2f(CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px"))),
        weapons_bar_({static_cast<float>(CONF<int>("graphics/window_width_px")),
                      static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        health_bar_({CONF<int>("graphics/window_width_px") - HEALTH_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                     CONF<int>("graphics/window_height_px") - HEALTH_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")}),
        time_bar_({TIME_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                   CONF<int>("graphics/window_height_px") - TIME_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")}),
        fps_text_("FPS: ", RM.getFont("editor"), 12),
        object_use_text_("[F] Use object", RM.getFont(), CONF<float>("graphics/use_text_size")),
        npc_talk_text_("[T] Talk to NPC", RM.getFont(), CONF<float>("graphics/use_text_size")),
        left_hud_({0.0f, static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        right_hud_({static_cast<float>(CONF<int>("graphics/window_width_px")),
                    static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        stats_hud_({0.0f, 0.0f}),
        small_backpack_hud_({static_cast<float>(CONF<int>("graphics/window_width_px")), 0.0f}),
        level_hud_({static_cast<float>(CONF<int>("graphics/window_width_px")) / 2.0f, 0.0f}),
        player_(nullptr),
        camera_(nullptr),
        theme_("../data/config/gui_theme.txt") {}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[UserInterface] player_ or camera_ is nullptr!");
    }
    health_bar_.setMaxHealth(player_->getMaxHealth());
    time_bar_.setMaxTime(CONF<float>("journal_max_time"));

    fps_text_.setFillColor(sf::Color::White);
    fps_text_.setPosition(FPS_X_, FPS_Y_);

    object_use_text_.setFillColor(sf::Color::White);
    npc_talk_text_.setFillColor(sf::Color::White);

    graphics.getWindow().setMouseCursorVisible(false);
    graphics.getWindow().setKeyRepeatEnabled(false);
    graphics.getCurrentView().zoom(1.0f / CONF<float>("graphics/global_zoom"));
    graphics.setCurrentView();
    camera_->setViewNormalSize(graphics.getWindow().getView().getSize());

    gui_ = std::make_unique<tgui::Gui>(graphics.getWindow());
//    gui_->setFont(RM.getFont());
    small_backpack_hud_.registerGui(gui_.get(), &theme_);
    full_hud_ = std::make_unique<FullHud>(gui_.get(), &theme_,
                                          sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                       static_cast<float>(CONF<int>("graphics/window_height_px"))});
    menu_ = std::make_unique<Menu>(gui_.get(), &theme_);

    tgui::ToolTip::setInitialDelay({});
    tgui::ToolTip::setDistanceToMouse({-tgui::ToolTip::getDistanceToMouse().x, tgui::ToolTip::getDistanceToMouse().y});

    menu_->doShow(true);
}

void UserInterface::registerPlayer(Player* player)
{
    player_ = player;
}

void UserInterface::registerCamera(Camera* camera)
{
    camera_ = camera;
}

void UserInterface::spawnAchievement(const std::string& title, const std::string& text, const std::string& tex)
{
    achievements_.emplace_back(sf::Vector2f{CONF<int>("graphics/window_width_px") - ACHIEVEMENTS_MARGIN_,
                                            (ACHIEVEMENTS_MARGIN_ + CONF<sf::Vector2f>("graphics/achievement_size").y) * (achievements_.size() + 1)},
                               title, text, &RM.getTexture(tex));
}

void UserInterface::handleEvents(graphics::Graphics& graphics, float time_elapsed)
{
    static sf::Event event;

    utils::eraseIf<Achievement>(achievements_, [time_elapsed](Achievement& a) { return !a.update(time_elapsed); });
    utils::eraseIf<Thought>(thoughts_, [time_elapsed](Thought& thought) { return !thought.update(time_elapsed); });
    utils::eraseIf<BonusText>(bonus_texts_, [time_elapsed](BonusText& text) { return !text.update(time_elapsed); });

    updatePlayerStates(time_elapsed);
    handleMouse(graphics.getWindow());
    handleKeys();

    blood_splash_.update(time_elapsed);

    auto special_object = Game::get().getCurrentSpecialObject();
    if (special_object != nullptr)
    {
        object_use_text_.setString(special_object->getTextToUse());
        auto object_use_text_rect = object_use_text_.getLocalBounds();
        object_use_text_.setOrigin(object_use_text_rect.left + object_use_text_rect.width/2.0f,
                                   object_use_text_rect.top  + object_use_text_rect.height/2.0f);

        object_use_text_.setPosition(special_object->getPosition() - sf::Vector2f{0.0f, OBJECT_USE_TEXT_OFFSET_Y_});
        object_use_text_.setFillColor(sf::Color::White);
    }
    else
    {
        object_use_text_.setFillColor(sf::Color::Transparent);
    }

    auto npc_talk = Game::get().getCurrentTalkableCharacter();
    if (npc_talk != nullptr)
    {
        auto npc_talk_text_rect = npc_talk_text_.getLocalBounds();
        npc_talk_text_.setOrigin(npc_talk_text_rect.left + npc_talk_text_rect.width/2.0f,
                                 npc_talk_text_rect.top  + npc_talk_text_rect.height/2.0f);

        npc_talk_text_.setPosition(npc_talk->getPosition() - sf::Vector2f{0.0f, OBJECT_USE_TEXT_OFFSET_Y_});
        npc_talk_text_.setFillColor(sf::Color::White);
    }
    else
    {
        npc_talk_text_.setFillColor(sf::Color::Transparent);
    }

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

                health_bar_.setPosition(event.size.width - HEALTH_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                                        event.size.height - HEALTH_BAR_Y_ * CONF<float>("graphics/user_interface_zoom"));

                time_bar_.setPosition(TIME_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                                        event.size.height - TIME_BAR_Y_ * CONF<float>("graphics/user_interface_zoom"));

                weapons_bar_.setPosition(event.size.width, event.size.height);

                left_hud_.setPosition(0.0f, event.size.height);
                right_hud_.setPosition(event.size.width, event.size.height);

                camera_->setViewNormalSize(graphics.getWindow().getView().getSize());
                blood_splash_.resizeWindow(visible_area);

                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (Game::get().getGameState() == Game::GameState::Normal)
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
                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Num1:
                    {
                        Game::get().getPlayer().useItem("health");
                        break;
                    }
                    case sf::Keyboard::Num2:
                    {
                        Game::get().getPlayer().useItem("more_speed");
                        break;
                    }
                    case sf::Keyboard::Num3:
                    {
                        Game::get().getPlayer().useItem("rag3");
                        break;
                    }
                    case sf::Keyboard::Q:
                    {
                        Game::get().getPlayer().sideStep(Player::SideStepDir::Left);
                        break;
                    }
                    case sf::Keyboard::E:
                    {
                        Game::get().getPlayer().sideStep(Player::SideStepDir::Right);
                        break;
                    }
                    case sf::Keyboard::F:
                    {
                        if (Game::get().getGameState() == Game::GameState::Normal)
                            Game::get().useSpecialObject();
                        break;
                    }
                    case sf::Keyboard::T:
                    {
                        if (Game::get().getGameState() == Game::GameState::Normal)
                            Game::get().talk();
                        break;
                    }
                    case sf::Keyboard::Escape:
                    {
                        if (Game::get().getGameState() == Game::GameState::Paused)
                        {
                            Game::get().setGameState(Game::GameState::Normal);
                            full_hud_->show(false);
                            windows_.clear();
                        }
                        else if (Game::get().getGameState() != Game::GameState::Menu)
                        {
                            Game::get().setGameState(Game::GameState::Paused);
                            full_hud_->show(true);
                        }

                        break;
                    }
                    case sf::Keyboard::R:
                    {
                        if (!Game::get().isJournalFreezed())
                            Game::get().setGameState(Game::GameState::Reverse);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                break;
            }
            case sf::Event::KeyReleased:
            {
                if (event.key.code == sf::Keyboard::R)
                {
                    if (Game::get().getGameState() == Game::GameState::Reverse)
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
    if (Game::get().getGameState() != Game::GameState::Menu)
    {
        graphics.setCurrentView();
        graphics.getWindow().draw(object_use_text_);
        graphics.getWindow().draw(npc_talk_text_);

        for (auto& thought : thoughts_)
        {
            graphics.draw(thought);
        }

        for (auto& bonus_text : bonus_texts_)
        {
            graphics.draw(bonus_text);
        }
    }

    graphics.setStaticView();

    if (Game::get().getGameState() != Game::GameState::Menu)
    {
        graphics.draw(blood_splash_);

        //graphics.draw(fps_text_);
        graphics.draw(left_hud_);
        graphics.draw(right_hud_);
        graphics.draw(health_bar_);
        graphics.draw(time_bar_);
        graphics.draw(weapons_bar_);
        graphics.draw(*full_hud_);
        graphics.draw(stats_hud_);
        graphics.draw(small_backpack_hud_);
        graphics.draw(level_hud_);
    }
    else
    {
        graphics.draw(*menu_);
    }

    for (auto& achievement : achievements_)
    {
        graphics.draw(achievement);
    }

    gui_->draw();
    graphics.draw(crosshair_);

    RM.setFontsSmoothAllowed(false);
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

    float max_speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        max_speed = RMGET<float>("characters", "player", "max_running_speed");
    else
        max_speed = RMGET<float>("characters", "player", "max_speed");


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        delta.x -= max_speed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        delta.x += max_speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        delta.y -= max_speed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        delta.y += max_speed;
    }

    if (player_->isAlive())
        player_->setVelocity(sf::Vector2f{delta.x, delta.y} * player_->getSpeedFactor());
}

inline void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
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

    if (player_->isAlive() && Game::get().getGameState() == Game::GameState::Normal)
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

inline void UserInterface::updatePlayerStates(float time_elapsed)
{
    weapons_bar_.update(player_->getWeapons(), player_->getCurrentWeapon(), time_elapsed);

    health_bar_.update(player_->getHealth(), time_elapsed);

    time_bar_.update(Game::get().getJournal().getDurationSaved(), time_elapsed);
    time_bar_.setFreeze(Game::get().isJournalFreezed() && Game::get().getGameState() != Game::GameState::Reverse);

    blood_splash_.updateLifeState(player_->getLifeState());

    auto& stats = Game::get().getStats();
    stats_hud_.update(stats.getEnemiesKilled(), stats.getCrystalsPicked(), time_elapsed);
    small_backpack_hud_.update(time_elapsed);
    level_hud_.update(stats.getLevel(), stats.getExp(), time_elapsed);
    full_hud_->update(time_elapsed);
    menu_->update(time_elapsed);
}

void UserInterface::spawnThought(Character* user, const std::string& text)
{
    utils::eraseIf<Thought>(thoughts_, [user](Thought& thought) { return thought.getFather() == user; });

    thoughts_.emplace_back(user, text, CONF<float>("thought_duration"));
}

void UserInterface::spawnBonusText(const sf::Vector2f& pos, const std::string& text)
{
    bonus_texts_.emplace_back(pos, text);
}

void UserInterface::spawnAcceptWindow(const std::string& text, const std::function<void()>& func)
{
    std::shared_ptr<AcceptWindow> window = std::make_shared<AcceptWindow>(gui_.get(), &theme_, text,
                                                                          sf::Vector2f(CONF<int>("graphics/window_width_px"),
                                                                                       CONF<int>("graphics/window_height_px")) / 2.0f,
                                                                          CONF<sf::Vector2f>("graphics/popup_size"));
    window->bindFunction(func);
    windows_.emplace_back(window);
}

void UserInterface::closeWindow(Window* window)
{
    utils::eraseIf<std::shared_ptr<Window>>(windows_, [window](std::shared_ptr<Window>& window_) { return window_.get() == window; });
}

void UserInterface::spawnNoteWindow(const std::string& text)
{
    windows_.emplace_back(std::make_shared<NoteWindow>(gui_.get(), &theme_, text,
                          sf::Vector2f(CONF<int>("graphics/window_width_px"),
                                       CONF<int>("graphics/window_height_px")) / 2.0f,
                          CONF<sf::Vector2f>("graphics/popup_size")));
    Game::get().setGameState(Game::GameState::Paused);
}

void UserInterface::startGame()
{
    menu_->doShow(false);
    Game::get().setGameState(Game::GameState::Normal);
    small_backpack_hud_.doShow(false);
}

void UserInterface::openMenu()
{
    menu_->doShow(true);
    Game::get().setGameState(Game::GameState::Menu);
    full_hud_->show(false);
    small_backpack_hud_.doShow(true);
}
