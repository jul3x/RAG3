//
// Created by jul3x on 16.03.21.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>
#include <R3E/system/Logger.h>

#include <common/ResourceManager.h>
#include <client/Client.h>


int main()
{
    LOG.setLogStatus(Logger::LogStatus::Full);

    CFG.initialize("../data/config/config.j3x");
    CFG.appendConfig("../data/config/graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");
    CFG.appendConfig("../data/config/paths.j3x", "paths");

    if (CONF<bool>("graphics/auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.set<int>("graphics/window_width_px", res.x);
        CFG.set<int>("graphics/window_height_px", res.y);
    }

    RM.lazyLoadTexture("animations/shot");
    RM.lazyLoadTexture("animations/explosion_1");
    RM.lazyLoadTexture("animations/explosion_2");
    RM.lazyLoadTexture("animations/explosion_3");
    RM.lazyLoadTexture("animations/teleportation");

    auto client = std::make_unique<Client>();
    client->start();

    return 0;
}