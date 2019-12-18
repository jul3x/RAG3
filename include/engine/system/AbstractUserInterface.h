//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H

#include <engine/system/Config.h>
#include <game/ui/WeaponsBar.h>
#include <game/ui/HealthBar.h>
#include <engine/objects/AbstractDrawableObject.h>


class AbstractUserInterface : public AbstractDrawableObject {

public:
    explicit AbstractUserInterface() = default;

    AbstractUserInterface(const AbstractUserInterface&) = delete;

    AbstractUserInterface& operator=(const AbstractUserInterface&) = delete;

    virtual void initialize() = 0;

    virtual void handleEvents() = 0;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};


#endif //RAG3_ENGINE_SYSTEM_ABSTRACTUSERINTERFACE_H
