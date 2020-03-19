//
// Created by jul3x on 15.03.20.
//

#ifndef RAG3_EDITOR_GRID_H
#define RAG3_EDITOR_GRID_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/Graphics.h>

using namespace r3e;

namespace editor {

    class Grid {
    public:
        Grid(float size_x, float size_y);

        void draw(graphics::Graphics &graphics);

        ~Grid() { array_.clear(); };

    private:
        sf::VertexArray array_;

        sf::Vector2f size_;

    };

}; // namespace editor

#endif //RAG3_EDITOR_GRID_H
