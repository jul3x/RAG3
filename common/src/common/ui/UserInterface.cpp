//
// Created by jul3x on 27.02.19.
//

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>
#include <common/misc/Journal.h>
#include <common/ui/menu/SettingsWindow.h>
#include <common/ui/UserInterface.h>
#include <common/ui/NoteWindow.h>
#include <common/Framework.h>


UserInterface::UserInterface(Framework* framework) :
        framework_(framework),
        weapons_bar_({static_cast<float>(CONF<int>("graphics/window_width_px")),
                      static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        health_bar_(
                {CONF<int>("graphics/window_width_px") - HEALTH_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                 CONF<int>("graphics/window_height_px") - HEALTH_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")},
                "health_bar"),
        fps_text_("FPS: ", RM.getFont("editor"), 12),
        object_use_text_("Use object", RM.getFont(), CONF<float>("graphics/use_text_size")),
        npc_talk_text_("Talk to NPC", RM.getFont(), CONF<float>("graphics/use_text_size")),
        left_hud_({0.0f, static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        right_hud_({static_cast<float>(CONF<int>("graphics/window_width_px")),
                    static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        time_bar_({TIME_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                   CONF<int>("graphics/window_height_px") - TIME_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")},
                  "time_bar"),
        speed_bar_({SPEED_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                    CONF<int>("graphics/window_height_px") -
                    SPEED_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")},
                   "speed_bar"),
        blood_splash_(framework,
                      sf::Vector2f(CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px"))),
        stats_hud_({0.0f, 0.0f}),
        small_backpack_hud_(framework, {static_cast<float>(CONF<int>("graphics/window_width_px")), 0.0f}),
        player_(nullptr),
        camera_(nullptr),
        theme_("../data/config/gui_theme.txt")
{
    health_bar_.setReversed(true);
}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    graphics.getWindow().setMouseCursorVisible(false);
    graphics.getWindow().setKeyRepeatEnabled(false);
    graphics.getCurrentView().zoom(1.0f / CONF<float>("graphics/GLOBAL_ZOOM"));
    graphics.setCurrentView();
    camera_->setViewNormalSize(graphics.getWindow().getView().getSize());

    gui_ = std::make_unique<tgui::Gui>(graphics.getWindow());
    small_backpack_hud_.registerGui(framework_);

    tgui::ToolTip::setInitialDelay({});
    tgui::ToolTip::setDistanceToMouse({-tgui::ToolTip::getDistanceToMouse().x, tgui::ToolTip::getDistanceToMouse().y});

    if (player_ != nullptr)
    {
        health_bar_.setMaxAmount(player_->getMaxHealth());
        time_bar_.setMaxAmount(CONF<float>("journal_max_time"));
        speed_bar_.setMaxAmount(player_->getMaxRunningFuel());
        object_use_text_.setFillColor(sf::Color::White);
        npc_talk_text_.setFillColor(sf::Color::White);
    }

    gui_->setFont(RM.getFont("default"));
    menu_ = std::make_unique<Menu>(framework_, this, gui_.get(), &theme_);
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
                                            (ACHIEVEMENTS_MARGIN_ + CONF<sf::Vector2f>("graphics/achievement_size").y) *
                                            (achievements_.size() + 1)},
                               title, text, &RM.getTexture(tex));
}

void UserInterface::update(graphics::Graphics& graphics, float time_elapsed)
{
    utils::eraseIfUpdated<Achievement>(achievements_, time_elapsed);
    utils::eraseIf<Message>(messages_, [this, time_elapsed](Message& msg) {
        if (!msg.update(time_elapsed))
        {
            rollMessagesUp();
            return true;
        }
        return false;
    });
    utils::eraseIfUpdated<Thought>(thoughts_, time_elapsed);
    utils::eraseIfUpdated<BonusText>(bonus_texts_, time_elapsed);

    updatePlayerStates(time_elapsed);
    handleMouse(graphics.getWindow());
    handleKeys();

    if (player_ != nullptr)
        UserInterface::applyMovement(player_, keys_pressed_);

    for (auto& arrow : tutorial_arrows_)
        arrow.second.update(time_elapsed);

    auto special_object = framework_->getCurrentSpecialObject();
    if (special_object != nullptr)
    {
        auto text = special_object->getTextToUse();
        if (text.empty())
            text = "Use object";
        object_use_text_.setString("[" + utils::keyToString(static_cast<sf::Keyboard::Key>(CONF<int>("controls/use")))
                                   + "] " + text);
        auto object_use_text_rect = object_use_text_.getLocalBounds();
        object_use_text_.setOrigin(object_use_text_rect.left + object_use_text_rect.width / 2.0f,
                                   object_use_text_rect.top + object_use_text_rect.height / 2.0f);

        object_use_text_.setPosition(special_object->getPosition() - sf::Vector2f{0.0f, OBJECT_USE_TEXT_OFFSET_Y_});
        object_use_text_.setFillColor(sf::Color::White);
    }
    else
    {
        object_use_text_.setFillColor(sf::Color::Transparent);
    }

    auto npc_talk = framework_->getCurrentTalkableCharacter();
    if (npc_talk != nullptr)
    {
        npc_talk_text_.setString("[" + utils::keyToString(static_cast<sf::Keyboard::Key>(CONF<int>("controls/talk")))
                                 + "] Talk to NPC");
        auto npc_talk_text_rect = npc_talk_text_.getLocalBounds();
        npc_talk_text_.setOrigin(npc_talk_text_rect.left + npc_talk_text_rect.width / 2.0f,
                                 npc_talk_text_rect.top + npc_talk_text_rect.height / 2.0f);

        npc_talk_text_.setPosition(npc_talk->getPosition() - sf::Vector2f{0.0f, TALK_TEXT_OFFSET_Y_});
        npc_talk_text_.setFillColor(sf::Color::White);
    }
    else
    {
        npc_talk_text_.setFillColor(sf::Color::Transparent);
    }

    blood_splash_.update(time_elapsed);
    handleEvents(graphics);
}

void UserInterface::handleEvents(graphics::Graphics& graphics)
{
    static sf::Event event;

    while (graphics.getWindow().pollEvent(event))
    {
        gui_->handleEvent(event);

        switch (event.type)
        {
            case sf::Event::Closed:
            {
                framework_->close();
                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (framework_->getGameState() == Framework::GameState::Normal)
                {
                    if (framework_->isNormalGameplay())
                        handleScrolling(event.mouseWheelScroll.delta);
                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                if (event.key.code == CONF<int>("controls/health"))
                    framework_->useItem("health");
                else if (event.key.code == CONF<int>("controls/more_speed"))
                    framework_->useItem("more_speed");
                else if (event.key.code == CONF<int>("controls/rag3"))
                    framework_->useItem("rag3");
                else if (event.key.code == CONF<int>("controls/dodge_left"))
                    framework_->getPlayer()->sideStep(Player::SideStepDir::Left);
                else if (event.key.code == CONF<int>("controls/dodge_right"))
                    framework_->getPlayer()->sideStep(Player::SideStepDir::Right);
                else if (event.key.code == CONF<int>("controls/use"))
                {
                    if (framework_->getGameState() == Framework::GameState::Normal)
                        framework_->useSpecialObject();
                }
                else if (event.key.code == CONF<int>("controls/talk"))
                {
                    if (framework_->getGameState() == Framework::GameState::Normal)
                        framework_->talk();
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    auto player_alive = framework_->getPlayer() == nullptr || framework_->getPlayer()->isAlive();
                    if (full_hud_->isShow() && player_alive)
                    {
                        framework_->setGameState(Framework::GameState::Normal);
                        full_hud_->show(false);
                        clearWindows();
                    }
                    else if (framework_->getGameState() != Framework::GameState::Menu)
                    {
                        framework_->setGameState(Framework::GameState::Paused);
                        full_hud_->show(true);
                    }
                }
                if (event.key.code == CONF<int>("controls/time_reversal"))
                {
                    if (!framework_->isJournalFreezed() && framework_->getGameState() == Framework::GameState::Normal)
                        framework_->setGameState(Framework::GameState::Reverse);
                }

                break;
            }
            case sf::Event::KeyReleased:
            {
                if (framework_->getGameState() == Framework::GameState::Menu && menu_->isOpen(Menu::Window::Settings))
                {
                    dynamic_cast<SettingsWindow&>(menu_->getWindow(Menu::Window::Settings)).setKey(event.key.code);
                }
                else
                {
                    if (event.key.code == CONF<int>("controls/time_reversal"))
                    {
                        if (framework_->getJournal() != nullptr &&
                            framework_->getGameState() == Framework::GameState::Reverse)
                            framework_->setGameState(Framework::GameState::Normal);
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

void UserInterface::draw(graphics::Graphics& graphics)
{
    if (framework_->getGameState() != Framework::GameState::Menu)
    {
        graphics.setCurrentView();
        graphics.draw(object_use_text_);
        graphics.draw(npc_talk_text_);

        for (auto& thought : thoughts_)
            graphics.draw(thought);

        for (auto& bonus_text : bonus_texts_)
            graphics.draw(bonus_text);

        for (auto& arrow : tutorial_arrows_)
            graphics.draw(arrow.second);
    }

    graphics.setStaticView();

    if (framework_->getGameState() != Framework::GameState::Menu)
    {
        graphics.draw(blood_splash_);

        //graphics.draw(fps_text_);
        graphics.draw(left_hud_);
        graphics.draw(right_hud_);
        graphics.draw(health_bar_);
        graphics.draw(time_bar_);
        graphics.draw(speed_bar_);
        graphics.draw(weapons_bar_);
        graphics.draw(*full_hud_);
        graphics.draw(stats_hud_);
        graphics.draw(small_backpack_hud_);

        for (auto& msg : messages_)
            graphics.draw(msg);
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

void UserInterface::handleScrolling(float delta)
{
    auto do_increase = delta > 0 ? 1 : -1;

    if (player_->isAlive())
        player_->switchWeapon(do_increase);
}

void UserInterface::handleKeys()
{
    keys_pressed_.clear();

    auto up_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/up"));
    auto left_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/left"));
    auto down_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/down"));
    auto right_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/right"));
    auto run_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/run"));

    if (sf::Keyboard::isKeyPressed(run_key))
        keys_pressed_.insert(Keys::Run);

    if (sf::Keyboard::isKeyPressed(left_key))
        keys_pressed_.insert(Keys::Left);
    else if (sf::Keyboard::isKeyPressed(right_key))
        keys_pressed_.insert(Keys::Right);

    if (sf::Keyboard::isKeyPressed(up_key))
        keys_pressed_.insert(Keys::Up);
    else if (sf::Keyboard::isKeyPressed(down_key))
        keys_pressed_.insert(Keys::Down);
}

void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);
    auto player_pos = player_->getPosition() - RMGET<sf::Vector2f>("characters", "henry", "map_offset");
    if (framework_->getGameState() == Framework::GameState::Normal &&
        utils::geo::circleCircle(mouse_world_pos, 0.0f, player_pos, CONF<float>("characters/crosshair_min_distance")))
    {
        mouse_world_pos = player_pos + CONF<float>("characters/crosshair_min_distance")
                                       * utils::geo::getNormalized(mouse_world_pos - player_pos);
        mouse_pos = graphics_window.mapCoordsToPixel(mouse_world_pos);
        sf::Mouse::setPosition(mouse_pos, graphics_window);
    }

    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);

    bool is_gui = false;
    is_left_mouse_pressed_ = false;

    for (const auto& widget : gui_->getWidgets())
    {
        if (widget->mouseOnWidget({static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y)}) &&
            widget->isVisible())
        {
            is_gui = true;
            break;
        }
    }

    if (player_->isAlive() && framework_->getGameState() == Framework::GameState::Normal)
    {
        player_->setWeaponPointing(mouse_world_pos);

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            is_left_mouse_pressed_ = true;
            if (player_->shot())
                camera_->setShaking();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && canZoomIn(is_gui))
        {
            zoomIn(mouse_world_pos);
        }
        else if (canZoomOut())
        {
            zoomOut();
        }
    }
}

void UserInterface::updatePlayerStates(float time_elapsed)
{
    weapons_bar_.update(player_->getWeapons(), player_->getCurrentWeapon(), time_elapsed);

    auto stats = framework_->getStats();
    if (stats != nullptr)
        right_hud_.update(framework_->getStats()->getLevel(), framework_->getStats()->getExp(), time_elapsed);

    if (player_ != nullptr)
    {
        right_hud_.setName(player_->getName());
        right_hud_.setCharacter(player_->getId());
    }

    health_bar_.setMaxAmount(player_->getMaxHealth());
    health_bar_.update(player_->getHealth(), time_elapsed);
    small_backpack_hud_.update(time_elapsed);

    auto journal = framework_->getJournal();
    if (journal != nullptr)
    {
        time_bar_.setMaxAmount(player_->getMaxTimeManipulation());
        time_bar_.update(std::min(framework_->getTimeManipulationFuel(), journal->getDurationSaved()),
                         time_elapsed);
        time_bar_.setFreeze(
                framework_->isJournalFreezed() && framework_->getGameState() != Framework::GameState::Reverse);
    }

    speed_bar_.setMaxAmount(player_->getMaxRunningFuel());
    speed_bar_.update(player_->getRunningFuel(), time_elapsed);
    speed_bar_.setFreeze(player_->getRunningFuel() < CONF<float>("running_min_time") && !player_->isRunning());
    blood_splash_.updateLifeState(player_->getLifeState());

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
    std::shared_ptr<AcceptWindow> window = std::make_shared<AcceptWindow>(this, text,
                                                                          sf::Vector2f(
                                                                                  CONF<int>("graphics/window_width_px"),
                                                                                  CONF<int>(
                                                                                          "graphics/window_height_px")) /
                                                                          2.0f,
                                                                          CONF<sf::Vector2f>
                                                                                  ("graphics/accept_window_size"));
    window->bindFunction(func);
    windows_.emplace_back(window);
}

void UserInterface::closeWindow(Window* window)
{
    utils::eraseIf<std::shared_ptr<Window>>(windows_, [window](std::shared_ptr<Window>& window_) {
        return window_.get() == window;
    });
    framework_->spawnSound(RM.getSound("ui_click"));

    if (framework_->getGameState() == Framework::GameState::Paused && !full_hud_->isShow())
        framework_->setGameState(Framework::GameState::Normal);
}

void UserInterface::spawnNoteWindow(const std::string& text, bool note_info)
{
    windows_.emplace_back(std::make_shared<NoteWindow>(framework_, this, text,
                                                       sf::Vector2f(CONF<int>("graphics/window_width_px"),
                                                                    CONF<int>("graphics/window_height_px")) / 2.0f,
                                                       CONF<sf::Vector2f>("graphics/popup_size"), note_info));
    framework_->spawnSound(RM.getSound("ui_click"));
}

void UserInterface::openMenu()
{
    menu_->doShow(true);
    framework_->setGameState(Framework::GameState::Menu);
    full_hud_->show(false);
    small_backpack_hud_.doShow(false);
}

void UserInterface::startGame()
{
    menu_->doShow(false);
    framework_->setGameState(Framework::GameState::Normal);
    small_backpack_hud_.doShow(true);
    menu_->showWindow(Menu::Window::None);
}

tgui::Gui* UserInterface::getGui()
{
    return gui_.get();
}

tgui::Theme* UserInterface::getTheme()
{
    return &theme_;
}

Framework* UserInterface::getFramework()
{
    return framework_;
}

const std::unordered_set<UserInterface::Keys>& UserInterface::getKeysPressed()
{
    return keys_pressed_;
}

bool UserInterface::isLeftMousePressed() const
{
    return is_left_mouse_pressed_;
}

void UserInterface::initializeTutorialArrows()
{
    tutorial_arrows_.clear();
    const auto& has_arrows = CONF<j3x::List>("graphics/tutorial_arrows");
    for (size_t i = 0; i < has_arrows.size(); ++i)
    {
        auto name = j3x::getObj<std::string>(has_arrows, i);

        if (name == "talkables")
        {
            for (auto& obj : framework_->getMap()->getList<NPC>())
            {
                if (obj->isTalkable())
                    tutorial_arrows_.emplace(std::make_pair(obj.get(), TutorialArrow(obj.get())));
            }
        }
        else
        {
            for (auto& obj : framework_->getMap()->getList<Special>())
            {
                if (obj->getId() == name)
                    tutorial_arrows_.emplace(std::make_pair(obj.get(), TutorialArrow(obj.get())));
            }
        }
    }
}

void UserInterface::removeArrowIfExists(AbstractPhysicalObject* obj)
{
    auto it = tutorial_arrows_.find(obj);
    if (it != tutorial_arrows_.end())
        tutorial_arrows_.erase(it);
}

void UserInterface::clearThoughts()
{
    thoughts_.clear();
}

void UserInterface::clearWindows()
{
    windows_.clear();
}

bool UserInterface::canZoomIn(bool is_mouse_on_gui) const
{
    return !is_mouse_on_gui;
}

bool UserInterface::canZoomOut() const
{
    return true;
}

void UserInterface::zoomIn(const sf::Vector2f& mouse_world_pos)
{
    camera_->setPointingTo(player_->getPosition() +
                           utils::geo::getNormalized(mouse_world_pos - player_->getPosition()) *
                           CONF<float>("graphics/camera_right_click_distance_factor"));
    camera_->setZoomTo(CONF<float>("graphics/camera_right_click_zoom_factor"));
}

void UserInterface::zoomOut()
{
    camera_->setPointingTo(player_->getPosition());
    camera_->setZoomTo(1.0f);
}

void UserInterface::applyMovement(Player* player, const std::unordered_set<UserInterface::Keys>& keys_pressed)
{
    auto delta = sf::Vector2f();
    if (keys_pressed.count(UserInterface::Keys::Run))
    {
        player->setRunning(true);
    }
    else
    {
        player->setRunning(false);
    }

    float max_speed = player->isRunning() ? RMGET<float>("characters", "henry", "max_running_speed") :
                      RMGET<float>("characters", "henry", "max_speed");

    if (keys_pressed.count(UserInterface::Keys::Left))
    {
        delta.x -= max_speed;
    }
    else if (keys_pressed.count(UserInterface::Keys::Right))
    {
        delta.x += max_speed;
    }

    if (keys_pressed.count(UserInterface::Keys::Up))
    {
        delta.y -= max_speed;
    }
    else if (keys_pressed.count(UserInterface::Keys::Down))
    {
        delta.y += max_speed;
    }

    if (player->isAlive())
        player->setVelocity(sf::Vector2f{delta.x, delta.y} * player->getSpeedFactor());
}

void UserInterface::showFullHud(bool show)
{
    full_hud_->show(show);
}

void UserInterface::spawnMessage(const std::string& text)
{
    static const auto base_pos = CONF<sf::Vector2f>("graphics/messages_base_pos");
    static const auto max_msgs = CONF<int>("graphics/messages_max_count");

    if (messages_.size() >= max_msgs)
    {
        messages_.erase(messages_.begin());
        rollMessagesUp();
    }

    messages_.emplace_back(
            base_pos + sf::Vector2f(0.0f, messages_.size() * CONF<float>("graphics/message_font_size") * 1.5f),
            text);
}

void UserInterface::rollMessagesUp()
{
    for (auto& msg : messages_)
    {
        msg.setPosition(msg.getPosition() - sf::Vector2f(0.0f, CONF<float>("graphics/message_font_size") * 1.5f));
    }
}

