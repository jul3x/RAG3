//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTGAME_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTGAME_H

#include <R3E/graphics/Graphics.h>
#include <R3E/objects/AbstractPhysicalObject.h>


namespace r3e {

    class AbstractGame {
    public:
        virtual void initialize() = 0;
        virtual void update(float time_elapsed) = 0;
        virtual void draw(graphics::Graphics& graphics) = 0;

        virtual void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) = 0;
        virtual void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) = 0;
        virtual void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) = 0;
        virtual void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) = 0;

    };

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTGAME_H
