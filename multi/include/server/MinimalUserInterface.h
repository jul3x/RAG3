//
// Created by jul3x on 16.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H
#define RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H

#include <R3E/system/AbstractUserInterface.h>


namespace r3e {

    class MinimalUserInterface : public AbstractUserInterface {

    public:
        MinimalUserInterface() {}
        virtual void initialize(graphics::Graphics& graphics) {}
        virtual void handleEvents(graphics::Graphics& graphics, float time_elapsed) {
            static sf::Event event;

            while (graphics.getWindow().pollEvent(event))
            {
            }
        }
        virtual void draw(graphics::Graphics& graphics) {}

    };

} // namespace r3e

#endif //RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H
