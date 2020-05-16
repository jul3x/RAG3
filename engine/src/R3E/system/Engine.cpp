//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <SFML/Audio/Listener.hpp>

#include <R3E/utils/Geometry.h>
#include <R3E/system/Engine.h>
#include <R3E/system/Collisions.h>


namespace r3e {

    Engine::Engine() : time_scale_factor_(1.0f),
                       graphics_(nullptr),
                       sound_manager_(nullptr),
                       collisions_(nullptr),
                       ui_(nullptr),
                       camera_(nullptr),
                       game_(nullptr)
    {

    }

    void Engine::initializeGraphics(const sf::Vector2i& size,
                                    const std::string& title,
                                    int style,
                                    const sf::Color& bg_color)
    {
        graphics_ = std::make_unique<graphics::Graphics>(size, title, style, bg_color);
    }

    void Engine::initializeSoundManager(float attenuation)
    {
        sound_manager_ = std::make_unique<audio::SoundManager>(attenuation);
        sf::Listener::setDirection(0.0f, 0.0f, -1.0f);
    }

    void Engine::initializeCollisions(const sf::Vector2f& size, float grid)
    {
        collisions_ = std::make_unique<Collisions>();
        collisions_->initialize(size, grid);
        collisions_on_ = true;
    }

    void Engine::turnOnCollisions()
    {
        collisions_on_ = true;
    }

    void Engine::turnOffCollisions()
    {
        collisions_on_ = false;
    }

    void Engine::registerUI(AbstractUserInterface* user_interface)
    {
        ui_ = user_interface;
    }

    void Engine::registerCamera(graphics::AbstractCamera* camera)
    {
        camera_ = camera;
    }

    void Engine::registerGame(AbstractGame* game)
    {
        game_ = game;
    }

    void Engine::spawnAnimationEvent(const std::shared_ptr<graphics::AnimationEvent>& event)
    {
        animation_events_.push_back(event);
    }

    void Engine::spawnSoundEvent(const sf::SoundBuffer& buffer, const sf::Vector2f& position, float volume)
    {
        sound_manager_->playSound(buffer, position, volume);
    }

    void Engine::spawnEffect(const std::shared_ptr<graphics::Effect>& effect)
    {
        effects_.push_back(effect);
    }

    void Engine::start()
    {
        game_->initialize();
        ui_->initialize(*graphics_);

        auto time_start = std::chrono::system_clock::now();

        while (graphics_->isWindowOpen())
        {
            frame_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now() - time_start).count() / 1000000000.0f;
            time_start = std::chrono::system_clock::now();

            if (frame_time_ > 1.0f / static_cast<float>(MINIMUM_FPS_))
            {
                std::cerr << "[Engine] Warning - minimum FPS limit exceeded" << std::endl;
                continue;
            }

            ui_->handleEvents(*graphics_, time_scale_factor_ * frame_time_);
            game_->update(time_scale_factor_ * frame_time_);

            if (collisions_ != nullptr && collisions_on_)
                collisions_->update(game_);

            updateAnimationEvents(time_scale_factor_ * frame_time_);
            updateEffects(time_scale_factor_ * frame_time_);

            draw();

            if (sound_manager_ != nullptr)
                sound_manager_->update(time_scale_factor_ * frame_time_);
        }
    }

    sf::Vector2i Engine::detectResolution()
    {
        int res_x = static_cast<int>(sf::VideoMode::getDesktopMode().width);
        int res_y = static_cast<int>(sf::VideoMode::getDesktopMode().height);

        std::cout << "[Engine] Screen resolution is: " << res_x << "px, " << res_y << "px." << std::endl;

        return {res_x, res_y};
    }

    void Engine::registerStaticObject(StaticObject* obj)
    {
        collisions_->insert(obj);
    }

    void Engine::registerDynamicObject(DynamicObject* obj)
    {
        collisions_->insert(obj);
    }

    void Engine::registerHoveringObject(HoveringObject* obj)
    {
        collisions_->insert(obj);
    }

    void Engine::deleteStaticObject(StaticObject* obj)
    {
        collisions_->erase(obj);
    }

    void Engine::deleteDynamicObject(DynamicObject* obj)
    {
        collisions_->erase(obj);
    }

    void Engine::deleteHoveringObject(HoveringObject* obj)
    {
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

    void Engine::updateEffects(float time_elapsed)
    {
        for (auto it = effects_.begin(); it != effects_.end(); ++it)
        {
            if ((*it)->update(time_elapsed))
            {
                auto next_it = std::next(it);
                effects_.erase(it);
                it = next_it;
            }
        }
    }

    void Engine::draw()
    {
        graphics_->clear();
        graphics_->setViewCenter(camera_->getViewCenter());
        graphics_->setViewSize(camera_->getViewSize());

        game_->draw(*graphics_);

        graphics_->setStaticView();

        for (auto& effect : effects_)
        {
            graphics_->draw(*effect);
        }

        ui_->draw(*graphics_);

        graphics_->setCurrentView();

        graphics_->display();
    }

    void Engine::setTimeScaleFactor(float factor)
    {
        time_scale_factor_ = factor;

        if (sound_manager_ != nullptr)
            sound_manager_->changePitch(factor);
    }

    void Engine::changeSoundListenerPosition(const sf::Vector2f& pos)
    {
        sf::Listener::setPosition(pos.x, 0.0f, pos.y);
    }

    float Engine::getCurrentFPS() const
    {
        return 1.0f / (frame_time_);
    }

    void Engine::drawSortedAnimationEvents()
    {
        for (const auto& animation : animation_events_)
        {
            graphics_->drawSorted(*animation);
        }
    }

    void Engine::drawAnimationEvents()
    {
        for (const auto& animation : animation_events_)
        {
            graphics_->draw(*animation);
        }
    }

    float Engine::setFPSLimit(int fps)
    {
        graphics_->getWindow().setFramerateLimit(fps);
    }

} // namespace r3e