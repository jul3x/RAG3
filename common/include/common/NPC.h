//
// Created by jul3x on 31.10.19.
//


#ifndef RAG3_COMMON_NPC_H
#define RAG3_COMMON_NPC_H

#include <functional>

#include <R3E/ai/AbstractAgent.h>

#include <common/Character.h>
#include <common/Special.h>


using namespace r3e;

class NPC : public Character, public ai::AbstractAgent {
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

    NPC(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    NPC(const sf::Vector2f& position, const std::string& id,
        const std::string& activation, const std::vector<std::string>& functions,
        const std::vector<std::string>& datas, int u_id = -1);

    void registerEnemy(const Character* enemy);

    void removeEnemy(const Character* enemy);

    void clearEnemies();

    void registerMapBlockage(const ai::MapBlockage* map_blockage);

    bool update(float time_elapsed) override;

    const sf::Vector2f& getStartPosition() const override;

protected:
    void handleEnemySelection();

    void handleVisibilityState();

    void handleActionState();

    void standardAI(float time_elapsed);

    void noneAI(float time_elapsed);

    sf::Vector2f findNearestSafeSpot(const sf::Vector2f& direction) const;

    static constexpr float MAX_DISTANCE_ = 1500.0f;
    static constexpr float WALLS_BETWEEN_CLOSE_ = 0.0f;
    static constexpr float WALLS_BETWEEN_FAR_ = 1.0f;

    std::list<const Character*> enemies_;
    const Character* current_enemy_;

    const ai::MapBlockage* map_blockage_;

    VisibilityState visibility_state_;
    ActionState action_state_;

    std::function<void(NPC*, float)> ai_function_;

};

#endif // RAG3_COMMON_NPC_H
