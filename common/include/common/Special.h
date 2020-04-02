//
// Created by jul3x on 19.03.20.
//


#ifndef RAG3_COMMON_SPECIAL_H
#define RAG3_COMMON_SPECIAL_H

#include <functional>

#include <SFML/Graphics/CircleShape.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

using namespace r3e;

class Special : public HoveringObject, public Identifiable, public Unique {
public:
    Special(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Special(const sf::Vector2f& position, const std::string& id,
            const std::string& activation, const std::string& function,
            const std::string& data, int u_id = -1);

    bool isDrawable() const;

    const std::string& getActivation() const;

    const std::string& getFunction() const;

    const std::string& getData() const;

    const std::string& getTextToUse() const;

    void setActivation(const std::string& str);

    void setFunction(const std::string& str);

    void setData(const std::string& str);

    void bindFunction(std::function<void(Special*, const std::string&)> func, const std::string& text);

    bool isActive() const;

    void deactivate();

    void use();

private:
    bool is_drawable_, is_active_;

    std::string activation_, function_, data_;

    std::function<void(Special*, const std::string&)> func_;

    const std::string* text_to_use_;

};

#endif // RAG3_COMMON_SPECIAL_H
