//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_OBJECTS_DECORATION_H
#define RAG3_OBJECTS_DECORATION_H

#include <objects/AbstractDrawableObject.h>


class Decoration : public AbstractDrawableObject {
public:
    Decoration(const sf::Vector2f &position,
               const int type_number);

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

};

#endif // RAG3_OBJECTS_DECORATION_H
