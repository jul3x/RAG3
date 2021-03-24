//
// Created by jul3x on 23.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H
#define RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H

#include <memory>

#include <R3E/objects/Lightable.h>
#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/graphics/LightPoint.h>

#include <common/misc/Functional.h>
#include <common/misc/Shootable.h>


using namespace r3e;

class Obstacle : public StaticObject, public Shootable, public Lightable, public Functional {
public:
    Obstacle(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Obstacle(const sf::Vector2f& position, const std::string& id,
             const std::string& activation, const j3x::List& functions,
             const j3x::List& datas, int u_id = -1);

    graphics::StaticShadow* getShadow() const;

    bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f) override;
    void setCurrentFrame(short int frame) override;
    void changeTexture(sf::Texture* texture, bool reset = false) override;

    void setPosition(const sf::Vector2f& pos) override;

    bool update(float time_elapsed) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::unique_ptr<graphics::StaticShadow> static_shadow_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H
