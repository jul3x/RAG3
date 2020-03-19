//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_ENGINE_H
#define RAG3_ENGINE_SYSTEM_ENGINE_H

#include <memory>
#include <unordered_set>
#include <list>

#include "../audio/SoundManager.h"
#include "../graphics/Graphics.h"
#include "../graphics/AbstractCamera.h"
#include "../graphics/AnimationEvent.h"
#include "AbstractUserInterface.h"
#include "AbstractGame.h"
#include "Collisions.h"
#include "../objects/AbstractPhysicalObject.h"


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

        float getCurrentFPS() const;

        void setTimeScaleFactor(float factor);

        void start();

        void drawAnimationEvents();

        void drawSortedAnimationEvents();

        static void changeSoundListenerPosition(const sf::Vector2f& pos);

        static sf::Vector2i detectResolution();

    private:
        void updateAnimationEvents(float time_elapsed);

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

        std::list<std::shared_ptr<graphics::AnimationEvent>> animation_events_;
    };

} // namespace r3e


#endif //RAG3_ENGINE_SYSTEM_ENGINE_H
