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

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());

    engine_->initializeGraphics(
            sf::Vector2i{CFG.getInt("window_width_px"), CFG.getInt("window_height_px")}, "Codename: Rag3",
            CFG.getInt("full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CFG.getInt("background_color")));
    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    map_->loadMap("test");

    for (auto& obstacle : map_->getObstacles())
        engine_->registerStaticObject(&obstacle);

    for (auto& decoration : map_->getDecorations())
        engine_->registerDrawableObject(&decoration);

    for (auto& enemy : map_->getEnemies())
        engine_->registerDynamicObject(&enemy);

    engine_->registerDynamicObject(player_.get());
}

void Game::update(float time_elapsed)
{
    map_->update(time_elapsed);

    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        engine_->registerDrawableObject(
            map_->spawnDecoration(player_->getPosition(), Decoration::Type::Blood));
        spawnExplosionAnimation(player_->getPosition(), 25.0f);
        player_->setDead();
        deleteDynamicObject(player_.get());
    }

    for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            deleteHoveringObject(it->get());
            auto next_it = std::next(it);
            bullets_.erase(it);
            it = next_it;
        }
    }

    camera_->update(player_->getPosition(), time_elapsed);
}

void Game::draw(Graphics& graphics)
{
    for (const auto& decoration : map_->getDecorations())
        graphics.draw(decoration);

    for (const auto& obstacle : map_->getObstacles())
        graphics.draw(obstacle);

    for (const auto& enemy : map_->getEnemies())
        graphics.draw(enemy);

    for (const auto& bullet : bullets_)
        graphics.draw(*bullet);

    if (player_->isAlive())
        graphics.draw(*player_);
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
    engine_->registerHoveringObject(bullets_.back().get());
}

void Game::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto obstacle = dynamic_cast<Obstacle*>(s_obj);
    obstacle->getShot(*bullet);
    spawnSparksAnimation(bullet->getPosition(), bullet->getRotation() - 90.0f,
                         static_cast<float>(std::pow(bullet->getDeadlyFactor(), 0.4f)));

    bullet->setDead();
}

void Game::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);
    character->getShot(*bullet);
    spawnSparksAnimation(bullet->getPosition(), bullet->getRotation() - 90.0f,
                         static_cast<float>(std::pow(bullet->getDeadlyFactor(), 0.4f)));

    bullet->setDead();
}

void Game::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
    // Nothing to do for now (maybe sounds?)
}

void Game::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
    // Nothing to do for now (maybe sounds?)
}

void Game::deleteStaticObject(StaticObject* s_obj)
{
    engine_->deleteStaticObject(s_obj);
}

void Game::deleteHoveringObject(HoveringObject* h_obj)
{
    engine_->deleteHoveringObject(h_obj);
}

void Game::deleteDynamicObject(DynamicObject* d_obj)
{
    engine_->deleteDynamicObject(d_obj);
}
