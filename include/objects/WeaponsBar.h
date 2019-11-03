//
// Created by jprolejko on 03.11.19.
//

#ifndef RAG3_OBJECTS_WEAPONSBAR_H
#define RAG3_OBJECTS_WEAPONSBAR_H

#include <objects/AbstractDrawableObject.h>


class WeaponsBar : public AbstractDrawableObject {
public:
    WeaponsBar(const sf::Vector2f &position);

    static constexpr float SIZE_X_ = 420.0f;
    static constexpr float SIZE_Y_ = 70.0f;

};

#endif // RAG3_OBJECTS_WEAPONSBAR_H
