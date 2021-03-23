//
// Created by jul3x on 16.03.21.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>
#include <R3E/system/Logger.h>

#include <common/ResourceManager.h>
#include <server/Server.h>


int main()
{
    LOG.setLogStatus(Logger::LogStatus::Full);

    CFG.initialize("../data/config/config.j3x");
    CFG.appendConfig("../data/config/graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");
    CFG.appendConfig("../data/config/paths.j3x", "paths");


    Server::get().start();

    return 0;
}