//
// Created by jprolejko on 26.02.19.
//

#include <system/Config.h>
#include <system/Engine.h>

int main()
{
    constexpr int FRAME_RATE = 60;

    CFG.initialize("data/config.j3x");

    if (CFG.getInt("auto_resolution"))
    {
        sf::Vector2i res = Engine::detectResolution();
        CFG.setInt("window_width_px", res.x);
        CFG.setInt("window_height_px", res.y);
    }

    ResourceManager::getInstance().lazyLoadTexture("spawn");
    ResourceManager::getInstance().lazyLoadTexture("explosion1");
    ResourceManager::getInstance().lazyLoadTexture("explosion2");
    ResourceManager::getInstance().lazyLoadTexture("explosion3");
    Engine::getInstance().update(FRAME_RATE);

    return 0;
}