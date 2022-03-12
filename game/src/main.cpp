//
// Created by jul3x on 26.02.19.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>
#include <R3E/system/Logger.h>

#include <common/ResourceManager.h>
#include <Game.h>


int main()
{
    LOG.setLogStatus(Logger::LogStatus::Full);

    CFG.initialize("../data/config/config.j3x");
    CFG.appendConfig("../data/config/paths.j3x", "paths");

    RM.copyFileIfNotExist("../data/config/user/default_graphics.j3x", "../data/config/user/graphics.j3x");
    RM.copyFileIfNotExist("../data/config/user/default_sound.j3x", "../data/config/user/sound.j3x");
    RM.copyFileIfNotExist("../data/config/user/default_controls.j3x", "../data/config/user/controls.j3x");
    RM.copyFileIfNotExist("../data/config/user/default_general.j3x", "../data/config/user/general.j3x");
    RM.copyFileIfNotExist("../data/config/user/default_save.j3x", "../data/config/user/save.j3x");
    RM.copyFileIfNotExist("../data/config/user/default_current_map.j3x", "../data/config/user/current_map.j3x");

    CFG.appendConfig("../data/config/user/general.j3x", "general");
    CFG.appendConfig("../data/config/user/controls.j3x", "controls");
    CFG.appendConfig("../data/config/user/graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/user/sound.j3x", "sound");
    CFG.appendConfig("../data/config/user/default_ranges.j3x", "ranges");
    CFG.appendConfig("../data/config/user/save.j3x", "save");

    sf::Vector2i res = {CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px")};
    if (CONF<bool>("graphics/auto_resolution"))
    {
        res = Engine::detectResolution();
        CFG.set<int>("graphics/window_width_px", res.x);
        CFG.set<int>("graphics/window_height_px", res.y);
    }
    j3x::Parameters graphics_context = {{"GLOBAL_ZOOM", Engine::detectZoom(res)}};
    CFG.appendConfig("../data/config/graphics.j3x", "graphics", graphics_context);
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");

    RM.lazyLoadTexture("animations/shot");
    RM.lazyLoadTexture("animations/explosion_1");
    RM.lazyLoadTexture("animations/explosion_2");
    RM.lazyLoadTexture("animations/explosion_3");
    RM.lazyLoadTexture("animations/teleportation");

    RM.lazyLoadTexture("blood_hud_1");
    RM.lazyLoadTexture("blood_hud_2");
    RM.lazyLoadTexture("blood_hud_3");

    auto game = std::make_unique<Game>();
    game->start();

    return 0;
}