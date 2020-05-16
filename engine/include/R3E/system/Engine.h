//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ENGINE_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ENGINE_H

#include <memory>
#include <unordered_set>
#include <list>

#include <R3E/audio/SoundManager.h>
#include <R3E/graphics/Graphics.h>
#include <R3E/graphics/AbstractCamera.h>
#include <R3E/graphics/AnimationEvent.h>
#include <R3E/graphics/Effect.h>
#include <R3E/system/AbstractUserInterface.h>
#include <R3E/system/AbstractGame.h>
#include <R3E/system/Collisions.h>
#include <R3E/objects/AbstractPhysicalObject.h>


namespace r3e {
    class Engine {

    public:
        Engine(const Engine&) = delete;

        Engine& operator=(const Engine&) = delete;

        Engine();

        void initializeGraphics(const sf::Vector2i& size,
                                const std::string& title,
                                int style,
                                const sf::Color& bg_color);
        void initializeSoundManager(float attenuation);
        void initializeCollisions(const sf::Vector2f& size, float grid);

        void turnOnCollisions();
        void turnOffCollisions();

        void registerUI(AbstractUserInterface* user_interface);
        void registerCamera(graphics::AbstractCamera* camera);
        void registerGame(AbstractGame* game);
        void registerStaticObject(StaticObject* obj);
        void registerDynamicObject(DynamicObject* obj);
        void registerHoveringObject(HoveringObject* obj);

        void deleteStaticObject(StaticObject* obj);
        void deleteDynamicObject(DynamicObject* obj);
        void deleteHoveringObject(HoveringObject* obj);

        void spawnAnimationEvent(const std::shared_ptr<graphics::AnimationEvent>& event);
        void spawnSoundEvent(const sf::SoundBuffer& buffer, const sf::Vector2f& position, float volume = 100.0f);
        void spawnEffect(const std::shared_ptr<graphics::Effect>& effect);

        [[nodiscard]] float getCurrentFPS() const;
        [[nodiscard]] static sf::Vector2i detectResolution();
        float setFPSLimit(int fps);

        void setTimeScaleFactor(float factor);
        void start();

        void drawAnimationEvents();
        void drawSortedAnimationEvents();
        static void changeSoundListenerPosition(const sf::Vector2f& pos);

    private:
        static constexpr int MINIMUM_FPS_ = 10;

        void updateAnimationEvents(float time_elapsed);
        void updateEffects(float time_elapsed);

        void draw();

        // Engine components
        std::unique_ptr<graphics::Graphics> graphics_;
        std::unique_ptr<audio::SoundManager> sound_manager_;
        std::unique_ptr<Collisions> collisions_;

        // Registered by Game
        AbstractUserInterface* ui_;
        graphics::AbstractCamera* camera_;
        AbstractGame* game_;

        float time_scale_factor_;
        float frame_time_;
        float collisions_on_;

        std::list<std::shared_ptr<graphics::AnimationEvent>> animation_events_;
        std::list<std::shared_ptr<graphics::Effect>> effects_;
    };

} // namespace r3e


#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ENGINE_H
