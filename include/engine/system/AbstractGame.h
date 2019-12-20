//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H

#include <engine/graphics/Graphics.h>
#include <engine/objects/AbstractPhysicalObject.h>


class AbstractGame {
public:
    virtual void initialize() = 0;
    virtual void update(float time_elapsed) = 0;
    virtual void draw(Graphics& graphics) = 0;

    virtual void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) = 0;
    virtual void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) = 0;
    virtual void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) = 0;
    virtual void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) = 0;

};

#endif //RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H
