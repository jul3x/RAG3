//
// Created by jul3x on 01.09.20.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H

#include <R3E/graphics/LightPoint.h>


namespace r3e {
    class Lightable {
    public:
        Lightable() = default;

        [[nodiscard]] graphics::LightPoint* getLightPoint() const {
            return light_.get();
        }

        void lightOff() {
            light_.reset();
        }

    protected:
        std::unique_ptr<graphics::LightPoint> light_;

    };
} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H
