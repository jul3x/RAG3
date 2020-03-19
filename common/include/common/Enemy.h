//
// Created by jul3x on 31.10.19.
//


#ifndef RAG3_COMMON_ENEMY_H
#define RAG3_COMMON_ENEMY_H

#include <R3E/ai/AbstractAgent.h>

#include <common/Character.h>


using namespace r3e;

class Enemy : public Character, public ai::AbstractAgent {
public:
    enum class VisibilityState {
        Close,
        Far,
        TooFar,
        OutOfRange
    };

    enum class ActionState {
        StandBy,
        Follow,
        DestroyWall,
        Shot,
        ShotAndRun,
        Run
    };

    Enemy(const sf::Vector2f& position, const std::string& id);

    void registerEnemy(const Character* enemy);

    void registerMapBlockage(const ai::MapBlockage* map_blockage);

    bool update(float time_elapsed, float time_factor);

    const sf::Vector2f& getStartPosition() const override;

private:
    inline void handleVisibilityState();

    inline void handleActionState();

    inline sf::Vector2f findNearestSafeSpot(const sf::Vector2f& direction) const;

    static constexpr float MAX_DISTANCE_ = 1500.0f;
    static constexpr float WALLS_BETWEEN_CLOSE_ = 0.0f;
    static constexpr float WALLS_BETWEEN_FAR_ = 1.0f;

    const Character* enemy_;
    const ai::MapBlockage* map_blockage_;

    VisibilityState visibility_state_;
    ActionState action_state_;

};

#endif // RAG3_COMMON_ENEMY_H