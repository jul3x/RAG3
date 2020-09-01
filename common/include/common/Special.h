//
// Created by jul3x on 19.03.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H
#define RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H

#include <functional>
#include <string>

#include <R3E/graphics/LightPoint.h>

#include <R3E/objects/Lightable.h>
#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

#include <common/Functional.h>


using namespace r3e;

class Special : public HoveringObject, public Lightable, public Functional {
public:
    Special(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Special(const sf::Vector2f& position, const std::string& id,
            const std::string& activation, const std::vector<std::string>& functions,
            const std::vector<std::string>& datas, bool is_active, int u_id = -1);

    [[nodiscard]] bool isDrawable() const;
    [[nodiscard]] bool getAdditionalBooleanData() const;
    graphics::StaticShadow* getShadow() const;

    bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f) override;
    void setCurrentFrame(short int frame) override;
    void changeTexture(sf::Texture* texture, bool reset = false) override;

    void setAdditionalBooleanData(bool data);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::unique_ptr<graphics::TransformedTextureShadow> static_shadow_;
    bool is_drawable_;

    bool additional_boolean_data_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_SPECIAL_H
