//
// Created by jul3x on 26.02.19.
//

#include <engine/system/Config.h>
#include <engine/system/Engine.h>
#include <game/misc/ResourceManager.h>
#include <game/Game.h>


int main()
{
    constexpr int FRAME_RATE = 60;

    CFG.initialize("../data/config.j3x");

    if (CFG.getInt("auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.setInt("window_width_px", res.x);
        CFG.setInt("window_height_px", res.y);
    }

    ResourceManager::getInstance().lazyLoadTexture("animation_shot");
    ResourceManager::getInstance().lazyLoadTexture("animation_explosion1");
    ResourceManager::getInstance().lazyLoadTexture("animation_explosion2");
    ResourceManager::getInstance().lazyLoadTexture("animation_explosion3");

    ResourceManager::getInstance().lazyLoadTexture("blood_hud_1");
    ResourceManager::getInstance().lazyLoadTexture("blood_hud_2");
    ResourceManager::getInstance().lazyLoadTexture("blood_hud_3");


    Game::get().start(FRAME_RATE);

    return 0;
}