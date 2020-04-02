//
// Created by jul3x on 23.03.20.
//

#ifndef RAG3_COMMON_OBSTACLE_H
#define RAG3_COMMON_OBSTACLE_H

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

#include <common/Shootable.h>


using namespace r3e;

class Obstacle : public StaticObject, public Shootable, public Identifiable, public Unique {
public:
    Obstacle(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    bool update(float time_elapsed) override;

};

#endif // RAG3_COMMON_OBSTACLE_H
