//
// Created by jul3x on 26.02.19.
//

#include <engine/system/Config.h>
#include <engine/system/Engine.h>
#include <game/misc/ResourceManager.h>


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

    std::unique_ptr<AbstractUserInterface> user_interface = std::make_unique<UserInterface>();
    std::unique_ptr<AbstractCamera> camera = std::make_unique<Camera>();

    Engine::getInstance().initializeGraphics(
            sf::Vector2i{CFG.getInt("window_width_px"), CFG.getInt("window_height_px")}, "Codename: Rag3",
            CFG.getInt("full_screen") ? sf::Style::Fullscreen : sf::Style::Default);
    Engine::getInstance().registerCamera(camera.get());
    Engine::getInstance().registerUI(user_interface.get());

    Engine::getInstance().update(FRAME_RATE);

    return 0;
}