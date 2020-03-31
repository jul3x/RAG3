//
// Created by jul3x on 19.03.20.
//


#ifndef RAG3_COMMON_SPECIAL_H
#define RAG3_COMMON_SPECIAL_H

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

using namespace r3e;

class Special : public AbstractDrawableObject, public Identifiable, public Unique {
public:
    Special(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Special(const sf::Vector2f& position, const std::string& id,
            const std::string& activation, const std::string& function,
            const std::string& data, int u_id = -1);

    const std::string& getActivation() const;

    const std::string& getFunction() const;

    const std::string& getData() const;

    void setActivation(const std::string& str);

    void setFunction(const std::string& str);

    void setData(const std::string& str);

private:
    std::string activation_, function_, data_;

};

#endif // RAG3_COMMON_SPECIAL_H
