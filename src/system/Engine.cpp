//
// Created by jprolejko on 27.02.19.
//

#include <chrono>

#include <system/Engine.h>


Engine::Engine() : player_({500.0f, 500.0f}, {}) {}

Player& Engine::getPlayer() {
    return player_;
}

void Engine::update(int frame_rate) {
    restartClock();
    auto time_start = std::chrono::system_clock::now();

    while (Graphics::getInstance().isWindowOpen())
    {
        float time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - time_start).count() / 1000000.0f;
        time_start = std::chrono::system_clock::now();

        ui_.handleEvents();

        physics_.update(time_elapsed);
        player_.update(time_elapsed);

        for (auto it = animation_events_.begin(); it != animation_events_.end(); ++it)
        {
            if ((*it)->update(time_elapsed))
            {
                auto next_it = std::next(it);
                animation_events_.erase(it);
                it = next_it;
            }
        }

        // drawing
        {
            Graphics::getInstance().clear();

            Graphics::getInstance().draw(player_);
            for (const auto &animation : animation_events_)
            {
                Graphics::getInstance().draw(*animation);
            }

            Graphics::getInstance().display();
        }

        ensureConstantFrameRate(frame_rate);
    }
}

sf::Vector2i Engine::detectResolution() {
    int res_x = static_cast<int>(sf::VideoMode::getDesktopMode().width);
    int res_y = static_cast<int>(sf::VideoMode::getDesktopMode().height);

    std::cout << "[Engine] Screen resolution is: " << res_x << "px, " << res_y << "px." << std::endl;

    return {res_x, res_y};
}

void Engine::ensureConstantFrameRate(const int frame_rate) {
    time_ = clock_.restart();
    sf::Time time_for_sleep =
        sf::milliseconds(static_cast<int>(1000.0f / static_cast<float>(frame_rate))) - time_;
    sf::sleep(time_for_sleep);
    time_ = clock_.restart();
}

void Engine::restartClock() {
    time_ = clock_.restart();
}