//
// Created by jul3x on 23.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H
#define RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H

#include <memory>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/graphics/LightPoint.h>

#include <common/Functional.h>
#include <common/Shootable.h>


using namespace r3e;

class Obstacle : public StaticObject, public Shootable, public Functional {
public:
    Obstacle(const sf::Vector2f& position, const std::string& id, int u_id = -1);

    Obstacle(const sf::Vector2f& position, const std::string& id,
             const std::string& activation, const std::vector<std::string>& functions,
             const std::vector<std::string>& datas, int u_id = -1);

    graphics::LightPoint* getLightPoint() const;

    bool update(float time_elapsed) override;

private:
    std::unique_ptr<graphics::LightPoint> light_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_OBSTACLE_H
