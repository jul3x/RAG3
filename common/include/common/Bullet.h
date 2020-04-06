//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_COMMON_BULLET_H
#define RAG3_COMMON_BULLET_H

#include <deque>

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
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr size_t TRAIL_COUNT_ = 10;

    std::deque<sf::Vector2f> trail_;
    sf::Color trail_color_;

    const float trail_time_step_;
    float trail_time_elapsed_;
    float life_;
    int deadly_factor_;

};

#endif // RAG3_COMMON_BULLET_H
