//
// Created by jprolejko on 27.02.19.
//

#include <chrono>

#include <utils/Geometry.h>
#include <graphics/ExplosionAnimation.h>
#include <graphics/ShotAnimation.h>
#include <graphics/SparksAnimation.h>
#include <system/ResourceManager.h>

#include <system/Engine.h>


Engine::Engine() : player_({400.0f, 500.0f}, {}) {
    map_.loadMap("map");
}

Player& Engine::getPlayer() {
    return player_;
}

void Engine::forceCameraShaking() {
    camera_.setShaking();
}

void Engine::spawnSparksAnimation(const sf::Vector2f &pos, const float dir, const float r) {
    animation_events_.push_back(
        std::make_unique<SparksAnimation>(pos, dir, r));
}

void Engine::spawnExplosionAnimation(const sf::Vector2f &pos, const float r) {
    animation_events_.push_back(
        std::make_unique<ExplosionAnimation>(pos, r));
}

void Engine::spawnShotAnimation(const sf::Vector2f &pos, const float dir, const float r) {
    animation_events_.push_back(
        std::make_unique<ShotAnimation>(pos, dir, r));
}

void Engine::spawnBullet(const std::string &name, const sf::Vector2f &pos, const float dir) {
    bullets_.emplace_back(ResourceManager::getInstance().getBulletDescription(name), pos, dir);
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

        map_.setVisibility(Graphics::getInstance().getCurrentView());
        player_.setVisibility(Graphics::getInstance().getCurrentView());

        map_.update(time_elapsed);
        if (player_.isVisible())
            player_.update(time_elapsed);

        auto visible_enemies = map_.getVisibleEnemies();
        auto visible_obstacles = map_.getVisibleObstacles();

        for (auto &obstacle : map_.getVisibleObstacles())
        {
            utils::AABBwithDS(player_, *obstacle);

            // obstacles -> enemies
            for (auto it_ob = visible_enemies.begin();
                 it_ob != visible_enemies.end(); ++it_ob)
            {
                utils::AABBwithDS(**it_ob, *obstacle);
            }
        }

        for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
        {
            bool remove_bullet = false;
            bool shooted = false;

            // bullet -> obstacle
            for (auto it_ob = visible_obstacles.begin(); it_ob != visible_obstacles.end(); ++it_ob)
            {
                if (utils::AABB(*it, **it_ob))
                {
                    (*it_ob)->getShot(*it);
                    shooted = true;

                    break;
                }
            }

            // bullet -> player
            if (utils::AABB(*it, player_))
            {
                player_.getShot(*it);
                shooted = true;
            }

            // bullet -> enemies
            for (auto it_ob = visible_enemies.begin(); it_ob != visible_enemies.end(); ++it_ob)
            {
                if (utils::AABB(*it, **it_ob))
                {
                    (*it_ob)->getShot(*it);
                    shooted = true;

                    break;
                }
            }

            if (shooted)
            {
                spawnSparksAnimation(it->getPosition(), it->getRotation() - 90.0f, std::pow(it->getDeadlyFactor(), 0.4f));
            }

            if (!it->update(time_elapsed) || shooted)
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

            for (const auto &bullet : bullets_)
            {
                Graphics::getInstance().draw(bullet);
            }

            Graphics::getInstance().draw(player_);

            for (const auto &animation : animation_events_)
            {
                Graphics::getInstance().draw(*animation);
            }

            Graphics::getInstance().setStaticView();
            Graphics::getInstance().draw(ui_);
            Graphics::getInstance().setCurrentView();

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