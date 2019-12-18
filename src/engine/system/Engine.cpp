//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <engine/utils/Geometry.h>
#include <game/animations/ExplosionAnimation.h>
#include <game/animations/ShotAnimation.h>
#include <game/animations/SparksAnimation.h>
#include <engine/system/AbstractResourceManager.h>

#include <engine/system/Engine.h>


// TODO - Make all of huge components constructors -> initialization -> loop -> destruction

Engine::Engine() : player_({400.0f, 500.0f}, {})
{
    map_.loadMap("map");
}

void Engine::initializeGraphics(const sf::Vector2i& size, const std::string& title, int style)
{
    graphics_ = std::make_unique<Graphics>(size, title, style);
}

void Engine::registerUI(AbstractUserInterface* user_interface)
{
    ui_ = user_interface;
}

void Engine::registerCamera(AbstractCamera* camera)
{
    camera_ = camera;
}

Graphics& Engine::getGraphics() const
{
    return *graphics_;
}

Player& Engine::getPlayer()
{
    return player_;
}

void Engine::forceCameraShaking()
{
    // NEEDED TO MOVE TO GAME CLASS IMMEDIATELY
    dynamic_cast<Camera*>(camera_)->setShaking();
}

void Engine::spawnSparksAnimation(const sf::Vector2f& pos, const float dir, const float r)
{
    animation_events_.push_back(
            std::make_unique<SparksAnimation>(pos, dir, r));
}

void Engine::spawnExplosionAnimation(const sf::Vector2f& pos, const float r)
{
    animation_events_.push_back(
            std::make_unique<ExplosionAnimation>(pos, r));
}

void Engine::spawnShotAnimation(const sf::Vector2f& pos, const float dir, const float r)
{
    animation_events_.push_back(
            std::make_unique<ShotAnimation>(pos, dir, r));
}

void Engine::spawnBullet(const std::string& name, const sf::Vector2f& pos, const float dir)
{
    bullets_.emplace_back(AbstractResourceManager::getInstance().getBulletDescription(name), pos, dir);
}

void Engine::update(int frame_rate)
{
    restartClock();

    ui_->initialize();

    auto time_start = std::chrono::system_clock::now();

    while (graphics_->isWindowOpen())
    {
        float time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - time_start).count() / 1000000.0f;
        time_start = std::chrono::system_clock::now();

        ui_->handleEvents();

        map_.setVisibility(graphics_->getCurrentView());
        player_.setVisibility(graphics_->getCurrentView());

        map_.update(time_elapsed);
        if (player_.isAlive() && !player_.update(time_elapsed))
        {
            map_.spawnDecoration(player_.getPosition(), Decoration::Type::Blood);
            spawnExplosionAnimation(player_.getPosition(), 25.0f);
            player_.setDead();
        }

        auto visible_enemies = map_.getVisibleEnemies();
        auto visible_obstacles = map_.getVisibleObstacles();

        for (auto& obstacle : map_.getVisibleObstacles())
        {
            utils::AABBwithDS(player_, *obstacle);

            // obstacles -> enemies
            for (auto& visible_enemy : visible_enemies)
            {
                utils::AABBwithDS(*visible_enemy, *obstacle);
            }
        }

        for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
        {
            bool remove_bullet = false;
            bool shooted = false;

            // bullet -> obstacle
            for (auto& visible_obstacle : visible_obstacles)
            {
                if (utils::AABB(*it, *visible_obstacle))
                {
                    visible_obstacle->getShot(*it);
                    shooted = true;

                    break;
                }
            }

            // bullet -> player
            if (player_.isAlive() && utils::AABB(*it, player_))
            {
                player_.getShot(*it);
                shooted = true;
            }

            // bullet -> enemies
            for (auto& visible_enemy : visible_enemies)
            {
                if (utils::AABB(*it, *visible_enemy))
                {
                    visible_enemy->getShot(*it);
                    shooted = true;

                    break;
                }
            }

            if (shooted)
            {
                spawnSparksAnimation(it->getPosition(), it->getRotation() - 90.0f,
                                     std::pow(it->getDeadlyFactor(), 0.4f));
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

        camera_->update(player_.getPosition(), time_elapsed);

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
            graphics_->clear();
            graphics_->setViewCenter(camera_->getViewCenter());

            graphics_->draw(map_);

            for (const auto& bullet : bullets_)
            {
                graphics_->draw(bullet);
            }

            if (player_.isAlive())
            {
                graphics_->draw(player_);
            }

            for (const auto& animation : animation_events_)
            {
                graphics_->draw(*animation);
            }

            graphics_->setStaticView();
            graphics_->draw(*ui_);
            graphics_->setCurrentView();

            graphics_->display();
        }

        ensureConstantFrameRate(frame_rate);
    }
}

sf::Vector2i Engine::detectResolution()
{
    int res_x = static_cast<int>(sf::VideoMode::getDesktopMode().width);
    int res_y = static_cast<int>(sf::VideoMode::getDesktopMode().height);

    std::cout << "[Engine] Screen resolution is: " << res_x << "px, " << res_y << "px." << std::endl;

    return {res_x, res_y};
}

void Engine::ensureConstantFrameRate(const int frame_rate)
{
    time_ = clock_.restart();
    sf::Time time_for_sleep =
            sf::milliseconds(static_cast<int>(1000.0f / static_cast<float>(frame_rate))) - time_;
    sf::sleep(time_for_sleep);
    time_ = clock_.restart();
}

void Engine::restartClock()
{
    time_ = clock_.restart();
}