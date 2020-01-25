//
// Created by jul3x on 25.01.2020.
//

#ifndef RAG3_ENGINE_SYSTEM_COLLISIONS_H
#define RAG3_ENGINE_SYSTEM_COLLISIONS_H

#include <engine/objects/AbstractPhysicalObject.h>


class Collisions {
public:
    Collisions() = delete;
    Collisions(const Collisions&) = delete;
    Collisions& operator=(const Collisions&) = delete;

    static bool ifCollidableResponse(DynamicObject& d_obj, const StaticObject& s_obj);

    static bool ifCollidableResponse(DynamicObject& d_obj, DynamicObject& s_obj);

    static bool areCollidable(const StaticObject& obj_1, const StaticObject& obj_2);

    static bool areCollidable(const DynamicObject& obj_1, const StaticObject& obj_2);

    static bool areCollidable(const DynamicObject& obj_1, const DynamicObject& obj_2);

};


#endif //RAG3_ENGINE_SYSTEM_COLLISIONS_H
