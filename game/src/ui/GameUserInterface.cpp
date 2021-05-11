//
// Created by jul3x on 27.02.19.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>

#include <ui/GameUserInterface.h>
#include <common/ui/NoteWindow.h>
#include <Game.h>


GameUserInterface::GameUserInterface(Game* game) :
        UserInterface(game),
        game_(game),
        blood_splash_(game,
                      sf::Vector2f(CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px"))),
        time_bar_({TIME_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                   CONF<int>("graphics/window_height_px") - TIME_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")},
                  "time_bar"),
        speed_bar_({SPEED_BAR_X_ * CONF<float>("graphics/user_interface_zoom"),
                   CONF<int>("graphics/window_height_px") - SPEED_BAR_Y_ * CONF<float>("graphics/user_interface_zoom")},
                  "speed_bar"),
        left_hud_({0.0f, static_cast<float>(CONF<int>("graphics/window_height_px"))}),
        stats_hud_({0.0f, 0.0f})
{

}

void GameUserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[UserInterface] player_ or camera_ is nullptr!");
    }

    UserInterface::initialize(graphics);
    health_bar_.setMaxAmount(player_->getMaxHealth());
    time_bar_.setMaxAmount(CONF<float>("journal_max_time"));
    speed_bar_.setMaxAmount(player_->getMaxRunningFuel());

    full_hud_ = std::make_unique<FullHud>(this, game_,
                                          sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                       static_cast<float>(CONF<int>("graphics/window_height_px"))});
    menu_ = std::make_unique<Menu>(framework_, this, gui_.get(), &theme_);
    menu_->doShow(true);
}

void GameUserInterface::update(graphics::Graphics& graphics, float time_elapsed)
{
    UserInterface::update(graphics, time_elapsed);

    blood_splash_.update(time_elapsed);
}

void GameUserInterface::handleAdditionalKeyPressed(sf::Keyboard::Key code)
{
    switch (code)
    {
        case sf::Keyboard::T:
        {
            if (game_->getGameState() == Game::GameState::Normal)
                game_->talk();
            break;
        }
        case sf::Keyboard::Escape:
        {
            if (game_->getGameState() == Game::GameState::Paused)
            {
                game_->setGameState(Game::GameState::Normal);
                full_hud_->show(false);
                windows_.clear();
            }
            else if (game_->getGameState() != Game::GameState::Menu)
            {
                game_->setGameState(Game::GameState::Paused);
                full_hud_->show(true);
            }

            break;
        }
        case sf::Keyboard::R:
        {
            if (!game_->isJournalFreezed() && game_->getGameState() == Game::GameState::Normal)
                game_->setGameState(Game::GameState::Reverse);
            break;
        }
        case sf::Keyboard::P:
        {
            if (game_->getGameState() == Game::GameState::Normal)
                game_->respawn();
            break;
        }
        default:
            break;
    }
}

void GameUserInterface::handleKeyReleased(sf::Keyboard::Key code)
{
    if (code == sf::Keyboard::R)
    {
        if (game_->getGameState() == Game::GameState::Reverse)
            game_->setGameState(Game::GameState::Normal);
    }
}


void GameUserInterface::draw(graphics::Graphics& graphics)
{
    if (game_->getGameState() != Game::GameState::Menu)
    {
        graphics.setCurrentView();
        graphics.draw(object_use_text_);
        graphics.draw(npc_talk_text_);

        for (auto& thought : thoughts_)
        {
            graphics.draw(thought);
        }

        for (auto& bonus_text : bonus_texts_)
        {
            graphics.draw(bonus_text);
        }

        for (auto& arrow : tutorial_arrows_)
        {
            graphics.draw(arrow.second);
        }
    }

    graphics.setStaticView();

    if (game_->getGameState() != Game::GameState::Menu)
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
//        graphics.draw(level_hud_);
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

void GameUserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    auto mouse_world_pos = graphics_window.mapPixelToCoords(mouse_pos);
    auto player_pos = player_->getPosition() - RMGET<sf::Vector2f>("characters", "player", "map_offset");
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

    if (player_->isAlive() && game_->getGameState() == Game::GameState::Normal)
    {
        player_->setWeaponPointing(mouse_world_pos);

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            is_left_mouse_pressed_ = true;
            if (player_->shot())
                camera_->setShaking();
        }

        if (!is_gui && sf::Mouse::isButtonPressed(sf::Mouse::Right) && game_->getTimeManipulationFuel() > 0.0f && game_->setBulletTime())
        {
            camera_->setPointingTo(player_->getPosition() +
                                   utils::geo::getNormalized(mouse_world_pos - player_->getPosition()) *
                                   CONF<float>("graphics/camera_right_click_distance_factor"));
            camera_->setZoomTo(CONF<float>("graphics/camera_right_click_zoom_factor"));
        }
        else if (game_->getForcedZoomTime() < 0.0f)
        {
            camera_->setPointingTo(player_->getPosition());
            camera_->setZoomTo(1.0f);
            game_->setNormalTime();
        }
    }
}

void GameUserInterface::updatePlayerStates(float time_elapsed)
{
    UserInterface::updatePlayerStates(time_elapsed);

    time_bar_.setMaxAmount(player_->getMaxTimeManipulation());
    time_bar_.update(std::min(game_->getTimeManipulationFuel(), game_->getJournal()->getDurationSaved()), time_elapsed);
    time_bar_.setFreeze(game_->isJournalFreezed() && game_->getGameState() != Game::GameState::Reverse);
    speed_bar_.setMaxAmount(player_->getMaxRunningFuel());
    speed_bar_.update(player_->getRunningFuel(), time_elapsed);
    speed_bar_.setFreeze(player_->getRunningFuel() < CONF<float>("running_min_time") && !player_->isRunning());
    blood_splash_.updateLifeState(player_->getLifeState());

    auto stats = game_->getStats();
    stats_hud_.update(stats->getEnemiesKilled(), stats->getCrystalsPicked(), time_elapsed);
    full_hud_->update(time_elapsed);
    menu_->update(time_elapsed);
}


void GameUserInterface::spawnNoteWindow(const std::string& text)
{
    UserInterface::spawnNoteWindow(text);
    game_->setGameState(Game::GameState::Paused);
}

void GameUserInterface::startGame()
{
    menu_->doShow(false);
    game_->setGameState(Game::GameState::Normal);
    small_backpack_hud_.doShow(false);
}

void GameUserInterface::openMenu()
{
    menu_->doShow(true);
    game_->setGameState(Game::GameState::Menu);
    full_hud_->show(false);
    small_backpack_hud_.doShow(true);
}
