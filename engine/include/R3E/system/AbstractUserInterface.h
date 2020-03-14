//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H

#include "../objects/AbstractDrawableObject.h"
#include "../graphics/Graphics.h"


namespace r3e {

    class AbstractUserInterface {

    public:
        explicit AbstractUserInterface() = default;

        AbstractUserInterface(const AbstractUserInterface&) = delete;

        AbstractUserInterface& operator=(const AbstractUserInterface&) = delete;

        virtual void initialize(graphics::Graphics& graphics) = 0;

        virtual void handleEvents(graphics::Graphics& graphics, float time_elapsed) = 0;

        virtual void draw(graphics::Graphics& graphics) = 0;

    };

} // namespace r3e

#endif //RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
