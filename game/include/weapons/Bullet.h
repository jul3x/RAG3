//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_GAME_WEAPONS_BULLET_H
#define RAG3_GAME_WEAPONS_BULLET_H

#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

struct BulletDescription {
    float speed_;
    float life_;
    int deadly_factor_;

    std::string name_;

    float size_x_;
    float size_y_;

    float burst_size_;
};


class Bullet : public HoveringObject {
public:
    Bullet(const BulletDescription& description,
           const sf::Vector2f& position,
           float direction);

    bool update(float time_elapsed) override;

    void setDead();

    int getDeadlyFactor() const;

private:
    static constexpr size_t TRAIL_COUNT_ = 4;

    float life_;
    int deadly_factor_;

};

#endif // RAG3_GAME_WEAPONS_PLAYER_H