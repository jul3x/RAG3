//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <engine/utils/Geometry.h>
#include <game/animations/ExplosionAnimation.h>
#include <game/animations/ShotAnimation.h>
#include <game/animations/SparksAnimation.h>
#include <game/misc/ResourceManager.h>

#include <game/Game.h>


Game::Game()
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Game::initialize()
{
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    player_ = std::make_unique<Player>(sf::Vector2f{400.0f, 500.0f}, sf::Vector2f{});
    map_ = std::make_unique<Map>();

    dynamic_cast<UserInterface*>(ui_.get())->registerCamera(dynamic_cast<Camera*>(camera_.get()));
    dynamic_cast<UserInterface*>(ui_.get())->registerPlayer(player_.get());

    engine_->initializeGraphics(
            sf::Vector2i{CFG.getInt("window_width_px"), CFG.getInt("window_height_px")}, "Codename: Rag3",
            CFG.getInt("full_screen") ? sf::Style::Fullscreen : sf::Style::Default);
    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    map_->loadMap("map");
}

void Game::update(float time_elapsed)
{
    engine_->setVisibility(*map_);
    engine_->setVisibility(*player_);

    map_->update(time_elapsed);
    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        map_->spawnDecoration(player_->getPosition(), Decoration::Type::Blood);
        spawnExplosionAnimation(player_->getPosition(), 25.0f);
        player_->setDead();
    }

    auto visible_enemies = map_->getVisibleEnemies();
    auto visible_obstacles = map_->getVisibleObstacles();

    for (auto& obstacle : map_->getVisibleObstacles())
    {
        utils::AABBwithDS(*player_, *obstacle);

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
            if (utils::AABB(**it, *visible_obstacle))
            {
                visible_obstacle->getShot(**it);
                shooted = true;

                break;
            }
        }

        // bullet -> player
        if (player_->isAlive() && utils::AABB(**it, *player_))
        {
            player_->getShot(**it);
            shooted = true;
        }

        // bullet -> enemies
        for (auto& visible_enemy : visible_enemies)
        {
            if (utils::AABB(**it, *visible_enemy))
            {
                visible_enemy->getShot(**it);
                shooted = true;

                break;
            }
        }

        if (shooted)
        {
            spawnSparksAnimation((*it)->getPosition(), (*it)->getRotation() - 90.0f,
                                 std::pow((*it)->getDeadlyFactor(), 0.4f));
        }

        if (!(*it)->update(time_elapsed) || shooted)
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

    camera_->update(player_->getPosition(), time_elapsed);
}

void Game::draw(Graphics& graphics)
{
    graphics.draw(*map_);

    for (const auto& bullet : bullets_)
    {
        graphics.draw(*bullet);
    }

    if (player_->isAlive())
    {
        graphics.draw(*player_);
    }
}

void Game::start(int frame_rate)
{
    engine_->update(frame_rate);
}

const sf::Vector2f& Game::getPlayerPosition() const
{
    return player_->getPosition();
}

void Game::spawnSparksAnimation(const sf::Vector2f& pos, const float dir, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<SparksAnimation>(pos, dir, r));
}

void Game::spawnExplosionAnimation(const sf::Vector2f& pos, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<ExplosionAnimation>(pos, r));
}

void Game::spawnShotAnimation(const sf::Vector2f& pos, const float dir, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<ShotAnimation>(pos, dir, r));
}

void Game::spawnBullet(const std::string& name, const sf::Vector2f& pos, const float dir)
{
    bullets_.emplace_back(
            std::make_unique<Bullet>(ResourceManager::getInstance().getBulletDescription(name), pos, dir));
}
