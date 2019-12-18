//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H

#include <engine/objects/AbstractDrawableObject.h>
#include <engine/graphics/Graphics.h>


class AbstractUserInterface : public AbstractDrawableObject {

public:
    explicit AbstractUserInterface() = default;

    AbstractUserInterface(const AbstractUserInterface&) = delete;

    AbstractUserInterface& operator=(const AbstractUserInterface&) = delete;

    virtual void initialize() = 0;

    virtual void handleEvents(Graphics &graphics) = 0;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;

};


#endif //RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
