//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_GAME_MISC_CAMERA_H
#define RAG3_GAME_MISC_CAMERA_H

#include <engine/graphics/AbstractCamera.h>


using namespace r3e;

class Camera : public graphics::AbstractCamera {
public:
    Camera();

    void setShaking();

    void update(float time_elapsed) override;

private:
    enum class State {
        NORMAL,
        SHOOTING
    };

    Camera::State state_;
};

#endif // RAG3_GAME_MISC_CAMERA_H