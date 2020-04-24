//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>

#include <Editor.h>

using namespace editor;

int main()
{
    CFG.initialize("../data/config/editor_config.j3x");
    CFG.appendConfig("../data/config/config.j3x", "");
    CFG.appendConfig("../data/config/graphics.j3x", "graphics");
    CFG.appendConfig("../data/config/sound.j3x", "sound");
    CFG.appendConfig("../data/config/characters.j3x", "characters");
    CFG.appendConfig("../data/config/paths.j3x", "paths");

    if (CFG.get<int>("auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.set<int>("window_width_px", res.x);
        CFG.set<int>("window_height_px", res.y);
    }

    Editor::get().start();

    return 0;
}