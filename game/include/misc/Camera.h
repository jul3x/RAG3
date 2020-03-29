//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_GAME_MISC_CAMERA_H
#define RAG3_GAME_MISC_CAMERA_H

#include <R3E/graphics/AbstractCamera.h>


using namespace r3e;

class Camera : public graphics::AbstractCamera {
public:
    Camera();

    void setShaking();
    
    void setZoomInOut();

    void setNormal();

    void setReverse();

    void update(float time_elapsed) override;

private:
    enum class State {
        Normal,
        Shooting,
        ZoomInOut,
        Reverse
    };

    Camera::State state_;
};

#endif // RAG3_GAME_MISC_CAMERA_H