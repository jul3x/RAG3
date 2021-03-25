//
// Created by jul3x on 16.03.21.
//

#include <iomanip>

#include <R3E/system/Engine.h>
#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>

#include <server/Server.h>
#include <server/MinimalUserInterface.h>


MinimalUserInterface::MinimalUserInterface(Server* server) : UserInterface(server) {}

void MinimalUserInterface::draw(graphics::Graphics& graphics)
{
}

inline void MinimalUserInterface::handleScrolling(float delta)
{
}

inline void MinimalUserInterface::handleKeys()
{
}

inline void MinimalUserInterface::handleMouse(sf::RenderWindow& graphics_window)
{
}

inline void MinimalUserInterface::updatePlayerStates(float time_elapsed)
{
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

void MinimalUserInterface::spawnNoteWindow(const std::string& text)
{
}

void MinimalUserInterface::closeWindow(Window* window)
{
}

