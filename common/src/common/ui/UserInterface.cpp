//
// Created by jul3x on 27.02.19.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>

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
        right_hud_({static_cast<float>(CONF<int>("graphics/window_width_px")),
                    static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        small_backpack_hud_(framework, {static_cast<float>(CONF<int>("graphics/window_width_px")), 0.0f}),
        player_(nullptr),
        camera_(nullptr),
        theme_("../data/config/gui_theme.txt")
{
    health_bar_.setReversed(true);
}

void UserInterface::initialize(graphics::Graphics& graphics)
{
    object_use_text_.setFillColor(sf::Color::White);
    npc_talk_text_.setFillColor(sf::Color::White);

    graphics.getWindow().setMouseCursorVisible(false);
    graphics.getWindow().setKeyRepeatEnabled(false);
    graphics.getCurrentView().zoom(1.0f / CONF<float>("graphics/global_zoom"));
    graphics.setCurrentView();
    camera_->setViewNormalSize(graphics.getWindow().getView().getSize());

    gui_ = std::make_unique<tgui::Gui>(graphics.getWindow());
    small_backpack_hud_.registerGui(framework_);

    tgui::ToolTip::setInitialDelay({});
    tgui::ToolTip::setDistanceToMouse({-tgui::ToolTip::getDistanceToMouse().x, tgui::ToolTip::getDistanceToMouse().y});
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
    utils::eraseIfUpdated<Thought>(thoughts_, time_elapsed);
    utils::eraseIfUpdated<BonusText>(bonus_texts_, time_elapsed);

    updatePlayerStates(time_elapsed);
    handleMouse(graphics.getWindow());
    handleKeys();

    for (auto& arrow : tutorial_arrows_)
        arrow.second.update(time_elapsed);

    auto special_object = framework_->getCurrentSpecialObject();
    if (special_object != nullptr)
    {
        object_use_text_.setString("[" + utils::keyToString(static_cast<sf::Keyboard::Key>(CONF<int>("controls/use")))
                                   + "] " + special_object->getTextToUse());
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
                    handleScrolling(event.mouseWheelScroll.delta);
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
                else
                    handleAdditionalKeyPressed(event.key.code);

                break;
            }
            case sf::Event::KeyReleased:
            {
                handleKeyReleased(event.key.code);
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
    auto delta = sf::Vector2f(0.0f, 0.0f);

    auto up_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/up"));
    auto left_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/left"));
    auto down_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/down"));
    auto right_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/right"));
    auto run_key = static_cast<sf::Keyboard::Key>(CONF<int>("controls/run"));

    if (sf::Keyboard::isKeyPressed(run_key))
    {
        keys_pressed_.insert(Keys::Run);
        player_->setRunning(true);
    }
    else
    {
        player_->setRunning(false);
    }

    float max_speed = player_->isRunning() ? RMGET<float>("characters", "player", "max_running_speed") :
                      RMGET<float>("characters", "player", "max_speed");

    if (sf::Keyboard::isKeyPressed(left_key))
    {
        delta.x -= max_speed;
        keys_pressed_.insert(Keys::Left);
    }
    else if (sf::Keyboard::isKeyPressed(right_key))
    {
        delta.x += max_speed;
        keys_pressed_.insert(Keys::Right);
    }

    if (sf::Keyboard::isKeyPressed(up_key))
    {
        delta.y -= max_speed;
        keys_pressed_.insert(Keys::Up);
    }
    else if (sf::Keyboard::isKeyPressed(down_key))
    {
        delta.y += max_speed;
        keys_pressed_.insert(Keys::Down);
    }

    if (player_->isAlive())
        player_->setVelocity(sf::Vector2f{delta.x, delta.y} * player_->getSpeedFactor());
}

void UserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
}

void UserInterface::updatePlayerStates(float time_elapsed)
{
    weapons_bar_.update(player_->getWeapons(), player_->getCurrentWeapon(), time_elapsed);

    auto stats = framework_->getStats();
    if (stats != nullptr)
        right_hud_.update(framework_->getStats()->getLevel(), framework_->getStats()->getExp(), time_elapsed);

    auto player = framework_->getPlayer();
    if (player != nullptr)
        right_hud_.setName(player->getName());

    health_bar_.setMaxAmount(player_->getMaxHealth());
    health_bar_.update(player_->getHealth(), time_elapsed);
    small_backpack_hud_.update(time_elapsed);
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
                                                                          CONF<sf::Vector2f>("graphics/popup_size"));
    window->bindFunction(func);
    windows_.emplace_back(window);
}

void UserInterface::closeWindow(Window* window)
{
    utils::eraseIf<std::shared_ptr<Window>>(windows_, [window](std::shared_ptr<Window>& window_) {
        return window_.get() == window;
    });
    framework_->spawnSound(RM.getSound("ui_click"));
}

void UserInterface::spawnNoteWindow(const std::string& text)
{
    windows_.emplace_back(std::make_shared<NoteWindow>(framework_, this, text,
                                                       sf::Vector2f(CONF<int>("graphics/window_width_px"),
                                                                    CONF<int>("graphics/window_height_px")) / 2.0f,
                                                       CONF<sf::Vector2f>("graphics/popup_size")));
    framework_->spawnSound(RM.getSound("ui_click"));
}

void UserInterface::openMenu()
{

}

void UserInterface::startGame()
{

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

const std::set<UserInterface::Keys>& UserInterface::getKeysPressed()
{
    return keys_pressed_;
}

bool UserInterface::isLeftMousePressed() const
{
    return is_left_mouse_pressed_;
}

void UserInterface::handleAdditionalKeyPressed(sf::Keyboard::Key code)
{

}

void UserInterface::handleKeyReleased(sf::Keyboard::Key code)
{

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

