//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_EDITOR_INCLUDE_EDITOR_CAMERA_H
#define RAG3_EDITOR_INCLUDE_EDITOR_CAMERA_H

#include <R3E/graphics/AbstractCamera.h>


using namespace r3e;

namespace editor {
    class Camera : public graphics::AbstractCamera {
    public:
        Camera();

        void update(float time_elapsed) override;

    };
} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_CAMERA_H