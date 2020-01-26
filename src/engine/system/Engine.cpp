//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <engine/utils/Geometry.h>
#include <engine/system/Engine.h>
#include <engine/system/Collisions.h>


void Engine::initializeGraphics(const sf::Vector2i& size,
                                const std::string& title,
                                int style,
                                const sf::Color &bg_color)
{
    graphics_ = std::make_unique<Graphics>(size, title, style, bg_color);
}

void Engine::initializeCollisions(const sf::Vector2f& size, float grid)
{
    collisions_ = std::make_unique<Collisions>();
    collisions_->initialize(size, grid);
}

void Engine::registerUI(AbstractUserInterface* user_interface)
{
    ui_ = user_interface;
}

void Engine::registerCamera(AbstractCamera* camera)
{
    camera_ = camera;
}

void Engine::registerGame(AbstractGame* game)
{
    game_ = game;
}

void Engine::spawnAnimationEvent(const std::shared_ptr<AnimationEvent>& event)
{
    animation_events_.push_back(event);
}

void Engine::update(int frame_rate)
{
    restartClock();

    game_->initialize();
    ui_->initialize();

    auto time_start = std::chrono::system_clock::now();

    while (graphics_->isWindowOpen())
    {
        float time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - time_start).count() / 1000000.0f;
        time_start = std::chrono::system_clock::now();

        setVisibilities();

        ui_->handleEvents(*graphics_);
        game_->update(time_elapsed);

        collisions_->update();
        DDCollisions(time_elapsed);
        HDCollisions(time_elapsed);
        DSCollisions(time_elapsed);
        HSCollisions(time_elapsed);

        updateAnimationEvents(time_elapsed);

        draw();

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

void Engine::setVisibility(AbstractDrawableObject& object) const
{
    object.setVisibility(graphics_->getCurrentView());
}

void Engine::setVisibilities() const
{
    for (auto &drawable : drawables_)
        drawable->setVisibility(graphics_->getCurrentView());

    for (auto &s_obj : s_objects_)
        s_obj->setVisibility(graphics_->getCurrentView());

    for (auto &h_obj : h_objects_)
        h_obj->setVisibility(graphics_->getCurrentView());

    for (auto &d_obj : d_objects_)
        d_obj->setVisibility(graphics_->getCurrentView());
}

void Engine::registerDrawableObject(AbstractDrawableObject* obj)
{
    drawables_.insert(obj);
}

void Engine::registerStaticObject(StaticObject* obj)
{
    s_objects_.insert(obj);
    collisions_->insert(obj);
}

void Engine::registerDynamicObject(DynamicObject* obj)
{
    d_objects_.insert(obj);
    collisions_->insert(obj);
}

void Engine::registerHoveringObject(HoveringObject* obj)
{
    h_objects_.insert(obj);
    collisions_->insert(obj);
}

void Engine::deleteDrawableObject(AbstractDrawableObject* obj)
{
    drawables_.erase(obj);
}

void Engine::deleteStaticObject(StaticObject* obj)
{
    s_objects_.erase(obj);
    collisions_->erase(obj);
}

void Engine::deleteDynamicObject(DynamicObject* obj)
{
    d_objects_.erase(obj);
    collisions_->erase(obj);
}

void Engine::deleteHoveringObject(HoveringObject* obj)
{
    h_objects_.erase(obj);
    collisions_->erase(obj);
}

void Engine::updateAnimationEvents(float time_elapsed)
{
    for (auto it = animation_events_.begin(); it != animation_events_.end(); ++it)
    {
        if ((*it)->update(time_elapsed))
        {
            auto next_it = std::next(it);
            animation_events_.erase(it);
            it = next_it;
        }
    }
}

void Engine::draw()
{
    graphics_->clear();
    graphics_->setViewCenter(camera_->getViewCenter());

    game_->draw(*graphics_);

    for (const auto& animation : animation_events_)
    {
        graphics_->draw(*animation);
    }

    graphics_->setStaticView();
    graphics_->draw(*ui_);
    graphics_->setCurrentView();

    graphics_->display();
}

void Engine::DSCollisions(float time_elapsed)
{
    for (auto& s_object : s_objects_)
    {
        for (auto& d_object : d_objects_)
        {
            if (Collisions::ifCollideResponse(*d_object, *s_object))
            {
                game_->alertCollision(d_object, s_object);
            }
        }
    }
}

void Engine::DDCollisions(float time_elapsed)
{
    for (auto& d_object_1 : d_objects_)
    {
        for (auto& d_object_2 : d_objects_)
        {
            if (d_object_1 != d_object_2 && Collisions::ifCollideResponse(*d_object_1, *d_object_2))
            {
                game_->alertCollision(d_object_1, d_object_2);
            }
        }
    }
}

void Engine::HSCollisions(float time_elapsed)
{
    for (auto& h_object : h_objects_)
    {
        for (auto& s_object : s_objects_)
        {
            if (Collisions::isCollision(*h_object, *s_object))
            {
                game_->alertCollision(h_object, s_object);
            }
        }
    }
}

void Engine::HDCollisions(float time_elapsed)
{
    for (auto& h_object : h_objects_)
    {
        for (auto& d_object : d_objects_)
        {
            if (Collisions::isCollision(*h_object, *d_object))
            {
                game_->alertCollision(h_object, d_object);
            }
        }
    }
}
