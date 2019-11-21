//
// Created by jprolejko on 16.10.19.
//


#ifndef RAG3_OBJECTS_ABSTRACTWEAPON_H
#define RAG3_OBJECTS_ABSTRACTWEAPON_H

#include <string>

#include <objects/AbstractDrawableObject.h>


class AbstractWeapon : public AbstractDrawableObject  {
public:
    AbstractWeapon(const sf::Vector2f &size,
                   const std::string &name);

    virtual sf::Vector2f use() = 0;

    const std::string& getName() const;

private:
    std::string name_;

};


#endif // RAG3_OBJECTS_ABSTRACTWEAPON_H