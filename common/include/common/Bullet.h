//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_BULLET_H
#define RAG3_COMMON_INCLUDE_COMMON_BULLET_H

#include <deque>

#include <R3E/objects/AbstractPhysicalObject.h>

#include <common/Functional.h>


using namespace r3e;

class Character;

class Bullet : public HoveringObject, public Functional {
public:
    Bullet(Character* user,
           const sf::Vector2f& position,
           const std::string& id,
           float direction);

    bool update(float time_elapsed) override;

    void setDead();

    float getDeadlyFactor() const;

    Character* getUser() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr size_t TRAIL_COUNT_ = 10;
    const float TRAIL_TIME_STEP_;

    std::deque<sf::Vector2f> trail_;
    sf::Color trail_color_;

    Character* user_;

    float trail_time_elapsed_;
    float life_;
    float deadly_factor_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_BULLET_H
