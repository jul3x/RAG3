//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_CROSSHAIR_H
#define RAG3_EDITOR_CROSSHAIR_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

namespace editor {
    class Crosshair : public AbstractDrawableObject {
    public:
        Crosshair();

    private:
        static constexpr float SIZE_X_ = 200.0f;
        static constexpr float SIZE_Y_ = 200.0f;
    };
} // namespace editor

#endif // RAG3_EDITOR_CROSSHAIR_H
