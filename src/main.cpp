//
// Created by jprolejko on 26.02.19.
//

#include <system/Config.h>
#include <system/Engine.h>
#include <system/ResourceManager.h>

int main()
{
    constexpr int FRAME_RATE = 60;

#ifdef __linux__
    CFG.initialize("data/config.j3x");
#elif _WIN32
    CFG.initialize("../data/config.j3x");
#endif


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
    Engine::getInstance().update(FRAME_RATE);

    return 0;
}