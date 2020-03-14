//
// Created by jul3x on 26.02.19.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>

#include <misc/ResourceManager.h>
#include <Game.h>


int main()
{
    CFG.initialize("../data/config/config.j3x");
    CFG.appendConfig("../data/config/graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");
    CFG.appendConfig("../data/config/paths.j3x", "paths");

    if (CFG.getInt("graphics/auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.setInt("graphics/window_width_px", res.x);
        CFG.setInt("graphics/window_height_px", res.y);
    }

    RM.lazyLoadTexture("animation_shot");
    RM.lazyLoadTexture("animation_explosion1");
    RM.lazyLoadTexture("animation_explosion2");
    RM.lazyLoadTexture("animation_explosion3");

    RM.lazyLoadTexture("blood_hud_1");
    RM.lazyLoadTexture("blood_hud_2");
    RM.lazyLoadTexture("blood_hud_3");


    Game::get().start();

    return 0;
}