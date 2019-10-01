//
// Created by jprolejko on 27.02.19.
//

#include <chrono>

#include <utils/Geometry.h>
#include <graphics/ExplosionAnimation.h>
#include <graphics/SpawnAnimation.h>

#include <system/Engine.h>


Engine::Engine() : player_({500.0f, 500.0f}, {}) {
    map_.loadMap("map");
}

Player& Engine::getPlayer() {
    return player_;
}

void Engine::forceCameraShaking() {
    camera_.setShaking();
}
void Engine::spawnExplosionAnimation(const sf::Vector2f &pos, const float r) {
    animation_events_.push_back(
        std::make_unique<ExplosionAnimation>(pos, r));
}

void Engine::spawnBullet(const sf::Vector2f &pos, const float dir) {
    static constexpr float LIFETIME = 600.0f;
    animation_events_.push_back(
        std::make_unique<SpawnAnimation>(pos, 3.0f));
    bullets_.emplace_back(pos, dir, LIFETIME);
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

        map_.setVisibility(Graphics::getInstance().getView());
        player_.setVisibility(Graphics::getInstance().getView());

        map_.update(time_elapsed);
        if (player_.isVisible())
            player_.update(time_elapsed);

        for (auto &obstacle : map_.getVisibleObstacles())
        {
            utils::AABBwithDS(player_, *obstacle);
        }

        for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
        {
            bool remove_bullet = false;

            for (auto it_ob = map_.getVisibleObstacles().begin();
                 it_ob != map_.getVisibleObstacles().end(); ++it_ob)
            {
                if (utils::AABBwithDS(*it, **it_ob))
                {
                    (*it_ob)->getShot();

                    remove_bullet = true;
                    animation_events_.push_back(
                        std::make_unique<SpawnAnimation>(it->getPosition(), 10.0f));

                    break;
                }
            }

            if (!it->updateBullet(time_elapsed))
            {
                remove_bullet = true;
            }

            if (remove_bullet)
            {
                auto next_it = std::next(it);
                bullets_.erase(it);
                it = next_it;
            }
        }

        camera_.update(player_.getPosition(), time_elapsed);

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
            Graphics::getInstance().setViewCenter(camera_.getViewCenter());

            Graphics::getInstance().draw(map_);

            Graphics::getInstance().draw(player_);

            for (const auto &bullet : bullets_)
            {
                Graphics::getInstance().draw(bullet);
            }

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