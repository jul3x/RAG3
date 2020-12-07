//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_GAME_INCLUDE_MISC_CAMERA_H
#define RAG3_GAME_INCLUDE_MISC_CAMERA_H

#include <R3E/graphics/AbstractCamera.h>


using namespace r3e;

class Camera : public graphics::AbstractCamera {
public:
    Camera();

    void setShaking(float factor = 1.0f);
    void setZoomInOut();
    void setNormal();
    void setReverse();
    void setRag3();

    void update(float time_elapsed) override;

private:
    enum class State {
        Normal,
        ZoomInOut,
        Reverse,
        Rag3,
    };

    std::unordered_map<float, float> shaking_map_;
    Camera::State state_;

};

#endif //RAG3_GAME_INCLUDE_MISC_CAMERA_H