//
// Created by jul3x on 23.03.20.
//

#include <R3E/system/Config.h>

#include <common/Obstacle.h>
#include <common/ResourceManager.h>


Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        StaticObject(position,
                     {utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_x"),
                      utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_y")},
                     Collision::Box(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_y")),
                     &RM.getTexture("obstacles/" + id),
                     utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "z_index"),
                     utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "frames_number"),
                     utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "frame_duration")),
        Shootable(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "endurance") *
                  CFG.get<float>("obstacles_endurance_factor"))
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_y")));
}

bool Obstacle::update(float time_elapsed)
{
    return life_ > 0;
}