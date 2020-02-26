//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H

#include <engine/objects/AbstractDrawableObject.h>
#include <engine/graphics/Graphics.h>

namespace r3e {

    class AbstractUserInterface : public AbstractDrawableObject {

    public:
        explicit AbstractUserInterface() = default;

        AbstractUserInterface(const AbstractUserInterface&) = delete;

        AbstractUserInterface& operator=(const AbstractUserInterface&) = delete;

        virtual void initialize(graphics::Graphics &graphics) = 0;

        virtual void handleEvents(graphics::Graphics& graphics, float time_elapsed) = 0;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;

    };

} // namespace r3e

#endif //RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
