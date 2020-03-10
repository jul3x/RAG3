//
// Created by jul3x on 21.02.20.
//

#ifndef RAG3_GAME_UI_CROSSHAIR_H
#define RAG3_GAME_UI_CROSSHAIR_H

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class Crosshair : public AbstractDrawableObject {
public:
    explicit Crosshair();

private:
    static constexpr float SIZE_X_ = 25.0f;
    static constexpr float SIZE_Y_ = 25.0f;
};

#endif // RAG3_GAME_UI_CROSSHAIR_H
