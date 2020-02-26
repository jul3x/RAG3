//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <engine/utils/Geometry.h>
#include <game/events/ExplosionEvent.h>
#include <game/events/ShotEvent.h>
#include <game/events/SparksEvent.h>
#include <game/misc/ResourceManager.h>

#include <game/Game.h>


Game::Game() : current_time_factor_(1.0f)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Game::initialize()
{
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    player_ = std::make_unique<Player>(sf::Vector2f{900.0f, 4100.0f}, sf::Vector2f{});
    map_ = std::make_unique<Map>();
    agents_manager_ = std::make_unique<ai::AgentsManager>(map_->getMapBlockage(), ai::AStar::EightNeighbours,
                                                          1000.0f, // max time without recalculation of path in ms
                                                          20.0f, // min change of goal to trigger recalculation
                                                          1000); // max search of path

    music_manager_ = std::make_unique<audio::MusicManager>();
    music_manager_->addDirectoryToQueue("../data/music");


    music_manager_->setVolume(50.0f);
    music_manager_->play();

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());

    engine_->initializeGraphics(
            sf::Vector2i{CFG.getInt("window_width_px"), CFG.getInt("window_height_px")}, "Codename: Rag3",
            CFG.getInt("full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CFG.getInt("background_color")));
    engine_->initializeSoundManager(CFG.getFloat("sound_attenuation"));

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    map_->loadMap("map");
    engine_->initializeCollisions(map_->getSize(), COLLISION_GRID_SIZE_);

    for (auto& obstacle : map_->getObstacles())
        engine_->registerStaticObject(&obstacle);

    for (auto& enemy : map_->getEnemies())
        engine_->registerDynamicObject(&enemy);

    engine_->registerDynamicObject(player_.get());
}

void Game::update(float time_elapsed)
{
    agents_manager_->update();
    map_->update(time_elapsed);

    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        map_->spawnDecoration(player_->getPosition(), Decoration::Type::Blood);
        spawnExplosionEvent(player_->getPosition(), 25.0f);
        player_->setDead();
        deleteDynamicObject(player_.get());
        music_manager_->stop();
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

    camera_->update(time_elapsed);

    Engine::changeSoundListenerPosition(player_->getPosition());
    music_manager_->update(time_elapsed);
}

void Game::draw(graphics::Graphics& graphics)
{
    for (auto& decoration : map_->getDecorations())
        graphics.draw(decoration);

    for (auto& obstacle : map_->getObstacles())
        graphics.draw(obstacle);

    for (auto& enemy : map_->getEnemies())
        graphics.draw(enemy);

    for (auto& bullet : bullets_)
        graphics.draw(*bullet);

    if (player_->isAlive())
        graphics.draw(*player_);
}

void Game::start()
{
    engine_->start();
}

const sf::Vector2f& Game::getPlayerPosition() const
{
    return player_->getPosition();
}

const ai::MapBlockage& Game::getMapBlockage() const
{
    return map_->getMapBlockage();
}

void Game::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<SparksEvent>(pos, dir, r));
}

void Game::spawnExplosionEvent(const sf::Vector2f& pos, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<ExplosionEvent>(pos, r));
    engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Game::spawnShotEvent(const std::string& name, const sf::Vector2f& pos, const float dir)
{
    auto shot_event = std::make_shared<ShotEvent>(pos, dir * 180.0f / M_PI,
                                                  RM.getBulletDescription(
                                                          name).burst_size_);
    engine_->spawnAnimationEvent(shot_event);
    engine_->spawnSoundEvent(RM.getSound(name + "_bullet_shot"), pos);
}

void Game::spawnBullet(const std::string& name, const sf::Vector2f& pos, const float dir)
{
    bullets_.emplace_back(
            std::make_unique<Bullet>(RM.getBulletDescription(name), pos, dir));
    engine_->registerHoveringObject(bullets_.back().get());

    this->spawnShotEvent(name, pos, dir);
}

void Game::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto obstacle = dynamic_cast<Obstacle*>(s_obj);
    obstacle->getShot(*bullet);
    spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f,
                     static_cast<float>(std::pow(bullet->getDeadlyFactor(), 0.4f)));

    bullet->setDead();
}

void Game::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);
    character->getShot(*bullet);
    spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f,
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

ai::AgentsManager& Game::getAgentsManager() const
{
    return *agents_manager_;
}

void Game::setBulletTime()
{
    current_time_factor_ = CFG.getFloat("bullet_time_factor");
    engine_->setTimeScaleFactor(current_time_factor_);
    music_manager_->setPlaybackPitch(CFG.getFloat("bullet_time_music_factor"));
}

void Game::setNormalTime()
{
    current_time_factor_ = 1.0f;
    engine_->setTimeScaleFactor(1.0f);
    music_manager_->setPlaybackPitch(1.0f);
}

float Game::getCurrentTimeFactor() const
{
    return current_time_factor_;
}

float Game::getFPS() const
{
    return engine_->getCurrentFPS();
}
