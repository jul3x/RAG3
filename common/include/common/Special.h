//
// Created by jul3x on 19.03.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H
#define RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H

#include <functional>
#include <string>

#include <R3E/graphics/LightPoint.h>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

#include <common/Functional.h>


using namespace r3e;

class Special : public HoveringObject, public Functional {
public:
    Special(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Special(const sf::Vector2f& position, const std::string& id,
            const std::string& activation, const std::vector<std::string>& functions,
            const std::vector<std::string>& datas, int u_id = -1);

    [[nodiscard]] bool isDrawable() const;
    [[nodiscard]] bool getAdditionalBooleanData() const;
    graphics::LightPoint* getLightPoint() const;

    void setAdditionalBooleanData(bool data);

private:
    std::unique_ptr<graphics::LightPoint> light_;
    bool is_drawable_;

    bool additional_boolean_data_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H
