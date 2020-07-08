//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_OBSTACLETILE_H
#define RAG3_COMMON_INCLUDE_COMMON_OBSTACLETILE_H

#include <memory>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/graphics/StaticShadow.h>

#include <common/Shootable.h>


using namespace r3e;

class ObstacleTile : public StaticObject, public Identifiable {
public:
    ObstacleTile(const sf::Vector2f& position, const std::string& id);

};

#endif //RAG3_COMMON_INCLUDE_COMMON_OBSTACLETILE_H
