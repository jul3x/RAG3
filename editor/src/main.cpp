//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>
#include <R3E/system/Engine.h>

#include <editor/ResourceManager.h>
#include <Editor.h>

using namespace editor;

int main()
{
    CFG.initialize("../data/config/editor_config.j3x");

    if (CFG.getInt("auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.setInt("window_width_px", res.x);
        CFG.setInt("window_height_px", res.y);
    }

    Editor::get().start();

    return 0;
}