//
// Created by jul3x on 16.03.21.
//

#include <server/Server.h>
#include <server/MinimalUserInterface.h>


MinimalUserInterface::MinimalUserInterface(Server* server) : UserInterface(server), server_(server)
{
}

void MinimalUserInterface::initialize(graphics::Graphics& graphics)
{
    UserInterface::initialize(graphics);
    menu_->makeMenuElements({
            {"Start server", [this]() { menu_->showWindow(Menu::Window::LoadGame); }},
            {"About",        [this]() { menu_->showWindow(Menu::Window::About); }},
            {"Exit",         [this]() { framework_->close(); }}
    });

    full_hud_ = std::make_unique<FullHud>(this, framework_,
                                          sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                       static_cast<float>(CONF<int>("graphics/window_height_px"))});
    menu_->doShow(true);
}

void MinimalUserInterface::draw(graphics::Graphics& graphics)
{
    graphics.setStaticView();

    if (framework_->getGameState() != Framework::GameState::Menu)
    {
        graphics.draw(*full_hud_);
    }
    else
    {
        graphics.draw(*menu_);
    }

    gui_->draw();
    graphics.draw(crosshair_);

    RM.setFontsSmoothAllowed(false);
}

inline void MinimalUserInterface::handleScrolling(float delta)
{
}

inline void MinimalUserInterface::handleKeys()
{
}

inline void MinimalUserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
    auto mouse_pos = sf::Mouse::getPosition(graphics_window);
    crosshair_.setPosition(mouse_pos.x, mouse_pos.y);
}

inline void MinimalUserInterface::updatePlayerStates(float time_elapsed)
{
    full_hud_->update(time_elapsed);
    menu_->update(time_elapsed);
}

void MinimalUserInterface::spawnAchievement(const std::string& title, const std::string& text, const std::string& tex)
{
}

void MinimalUserInterface::spawnThought(Character* user, const std::string& text)
{
}

void MinimalUserInterface::spawnBonusText(const sf::Vector2f& pos, const std::string& text)
{
}

void MinimalUserInterface::spawnAcceptWindow(const std::string& text, const std::function<void()>& func)
{
}

void MinimalUserInterface::spawnNoteWindow(const std::string& text, bool note_info)
{
}

void MinimalUserInterface::closeWindow(Window* window)
{
}

void MinimalUserInterface::openMenu()
{
    server_->disconnect();
    UserInterface::openMenu();
}
