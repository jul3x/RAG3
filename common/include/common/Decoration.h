//
// Created by jul3x on 20.03.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_DECORATION_H
#define RAG3_COMMON_INCLUDE_COMMON_DECORATION_H

#include <memory>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Lightable.h>
#include <R3E/objects/Unique.h>
#include <R3E/graphics/LightPoint.h>


using namespace r3e;

class Decoration : public AbstractDrawableObject, public Identifiable, public Lightable, public Unique {
public:
    Decoration(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    bool isActive() const;

    void deactivate();

private:
    bool is_active_;
};

#endif //RAG3_COMMON_INCLUDE_COMMON_DECORATION_H
