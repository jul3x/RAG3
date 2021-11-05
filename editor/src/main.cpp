//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>

#include <Editor.h>


using namespace editor;

int main()
{
    LOG.setLogStatus(Logger::LogStatus::Full);

    CFG.initialize("../data/config/editor_config.j3x");
    CFG.appendConfig("../data/config/config.j3x", "");

    sf::Vector2i res = {CFG.get<int>("window_width_px"), CFG.get<int>("window_height_px")};

    if (CONF<bool>("auto_resolution"))
    {
        res = Engine::detectResolution();
        CFG.set<int>("window_width_px", res.x);
        CFG.set<int>("window_height_px", res.y);
    }
    CFG.appendConfig("../data/config/graphics.j3x", "graphics", {{"GLOBAL_ZOOM", Engine::detectZoom(res)}});
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");
    CFG.appendConfig("../data/config/user/default_graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/user/default_sound.j3x", "sound");
    CFG.appendConfig("../data/config/paths.j3x", "paths");
    Editor::get().start();

    return 0;
}