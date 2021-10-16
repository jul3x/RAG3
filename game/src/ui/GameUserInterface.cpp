//
// Created by jul3x on 27.02.19.
//

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
    menu_->makeMenuElements({
        {"Start game", [this]() { framework_->startGame(); }},
        {"Load game",  [this]() { menu_->showWindow(Menu::Window::LoadGame); }},
        {"Settings",   [this]() { menu_->showWindow(Menu::Window::Settings); }},
        {"About",      [this]() { menu_->showWindow(Menu::Window::About); }},
        {"Exit",       [this]() { framework_->close(); }}
    });

    full_hud_ = std::make_unique<ExtendedFullHud>(this, framework_,
                                          sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                       static_cast<float>(CONF<int>("graphics/window_height_px"))});
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
