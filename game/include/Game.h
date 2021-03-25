//
// Created by jul3x on 18.12.19.
//

#ifndef RAG3_GAME_INCLUDE_GAME_H
#define RAG3_GAME_INCLUDE_GAME_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/audio/MusicManager.h>
#include <R3E/graphics/Lightning.h>

#include <common/Framework.h>
#include <common/misc/Journal.h>

#include <common/misc/Thought.h>
#include <misc/Achievements.h>
#include <misc/Stats.h>

#include <ui/GameUserInterface.h>


using namespace r3e;

class Game : public Framework {

public:
    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Engine methods
    void initialize() override;
    void update(float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;

    // Getters
    [[nodiscard]] Player* getPlayer() override;
    [[nodiscard]] Journal* getJournal() override;
    [[nodiscard]] Special* getCurrentSpecialObject() const override;
    [[nodiscard]] Character* getCurrentTalkableCharacter() const override;
    [[nodiscard]] float getRag3Time() const override;
    [[nodiscard]] float getForcedZoomTime() const;

    // Spawn events
    DestructionSystem* spawnSparksEvent(const sf::Vector2f& pos, float dir, float r) override;
    DestructionSystem* spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor) override;
    void spawnAchievement(const j3x::Parameters& params);
    void spawnThought(Character* user, const std::string& text) override;
    void spawnBonusText(const sf::Vector2f& pos, const std::string& text);

    // Spawn objects for journal
    Fire* spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir) override;
    Bullet* spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir) override;
    Special* spawnSpecial(const sf::Vector2f& pos, const std::string& name) override;
    Decoration* spawnDecoration(const sf::Vector2f& pos, const std::string& name) override;

    // UI functions
    void talk();
    void useSpecialObject() override;
    void setBulletTime();
    void setNormalTime();
    void setRag3Time(float time_elapsed) override;
    void forceZoomTo(AbstractPhysicalObject* obj) override;

    // Journal methods
    [[nodiscard]] bool isJournalFreezed() const;

    NPC* spawnNewNPC(const std::string& id, int u_id, Functional::Activation activation,
                     const j3x::List& funcs, const j3x::List& datas) override;
    NPC* spawnNewPlayerClone(const std::string& weapon_id) override;

    void setGameState(Framework::GameState state) override;

private:
    void updateMapObjects(float time_elapsed) override;
    void updatePlayerClone(float time_elapsed);
    void updatePlayer(float time_elapsed);
    void updateFire(float time_elapsed) override;
    void updateDestructionSystems(float time_elapsed) override;

    void initPlayers() override;
    void initNPCs() override;

    void killNPC(NPC* npc);
    void cleanPlayerClone();

    std::unique_ptr<audio::MusicManager> music_manager_;

    std::unique_ptr<Achievements> achievements_;
    std::unique_ptr<Journal> journal_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<PlayerClone> player_clone_;

    float rag3_time_elapsed_;
    float forced_zoom_to_time_elapsed_;
    AbstractPhysicalObject* current_obj_zoom_;
    float current_time_factor_;

};


#endif //RAG3_GAME_INCLUDE_GAME_H
