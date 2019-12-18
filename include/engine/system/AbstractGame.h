//
// Created by jul3x on 18.12.2019.
//

#ifndef RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H
#define RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H

#include <engine/graphics/Graphics.h>


class AbstractGame {
public:
    virtual void initialize() = 0;
    virtual void update(float time_elapsed) = 0;
    virtual void draw(Graphics& graphics) = 0;

};

#endif //RAG3_ENGINE_SYSTEM_ABSTRACTGAME_H
