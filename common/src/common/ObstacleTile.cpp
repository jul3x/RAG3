//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>

#include <common/ObstacleTile.h>
#include <common/ResourceManager.h>


ObstacleTile::ObstacleTile(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        StaticObject(position,
                     {utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "size_x"),
                      utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "size_y")},
                     collision::Box(
                             utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "collision_size_x"),
                             utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "collision_size_y"),
                             {utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "collision_offset_x"),
                              utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "collision_offset_y")}),
                     &RM.getTexture("obstacles_tiles/" + id),
                     utils::j3x::get<int>(RM.getObjectParams("obstacles_tiles", id), "z_index"),
                     0, 0.0f),
        Shootable(utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "endurance") *
                  CFG.get<float>("obstacles_endurance_factor"))
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "map_offset_y")));

    auto shadow_pos = this->getPosition() -
                      sf::Vector2f{utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "map_offset_x"),
                                   utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", id), "map_offset_y")};
    static_shadow_ = std::make_unique<graphics::StaticShadow>(
            shadow_pos, this->getSize(), CFG.get<float>("graphics/shadow_direction"),
            CFG.get<float>("graphics/shadow_length_factor"),
            &RM.getTexture("obstacles_tiles/" + id), sf::Color(CFG.get<int>("graphics/shadow_color")),
            z_index_ - 1);
}

bool ObstacleTile::update(float time_elapsed)
{
    return life_ > 0;
}

graphics::StaticShadow* ObstacleTile::getShadow() const
{
    return static_shadow_.get();
}
