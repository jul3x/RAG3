//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_COMMON_BULLET_H
#define RAG3_COMMON_BULLET_H

#include <R3E/objects/AbstractPhysicalObject.h>

#include <R3E/objects/Identifiable.h>


using namespace r3e;


class Bullet : public HoveringObject, public Identifiable {
public:
    Bullet(const sf::Vector2f& position,
           const std::string& id,
           float direction);

    bool update(float time_elapsed) override;

    void setDead();

    int getDeadlyFactor() const;

private:
    static constexpr size_t TRAIL_COUNT_ = 4;

    float life_;
    int deadly_factor_;

};

#endif // RAG3_COMMON_BULLET_H
