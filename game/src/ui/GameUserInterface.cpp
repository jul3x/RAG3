//
// Created by jul3x on 27.02.19.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>
#include <common/ui/NoteWindow.h>

#include <common/ui/menu/SettingsWindow.h>
#include <ui/GameUserInterface.h>
#include <Game.h>


GameUserInterface::GameUserInterface(Game* game) :
        UserInterface(game),
        game_(game)
{
}

void GameUserInterface::initialize(graphics::Graphics& graphics)
{
    if (player_ == nullptr || camera_ == nullptr)
    {
        throw std::runtime_error("[UserInterface] player_ or camera_ is nullptr!");
    }

    UserInterface::initialize(graphics);
    menu_->doShow(true);
}

void GameUserInterface::updatePlayerStates(float time_elapsed)
{
    UserInterface::updatePlayerStates(time_elapsed);

    auto stats = game_->getStats();
    stats_hud_.update(stats->getEnemiesKilled(), stats->getCrystalsPicked(), time_elapsed);
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
    small_backpack_hud_.doShow(true);
    menu_->showWindow(Menu::Window::None);
}

void GameUserInterface::openMenu()
{
    menu_->doShow(true);
    game_->setGameState(Game::GameState::Menu);
    full_hud_->show(false);
    small_backpack_hud_.doShow(false);
}

bool GameUserInterface::canZoomIn(bool mouse_on_widget) const
{
    return UserInterface::canZoomIn(mouse_on_widget) && game_->getTimeManipulationFuel() > 0.0f &&
           game_->setBulletTime();
}

bool GameUserInterface::canZoomOut() const
{
    return UserInterface::canZoomOut() && game_->getForcedZoomTime() < 0.0f;
}

void GameUserInterface::zoomOut()
{
    UserInterface::zoomOut();
    game_->setNormalTime();
}
