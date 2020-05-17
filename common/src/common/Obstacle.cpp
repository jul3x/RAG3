//
// Created by jul3x on 23.03.20.
//

#include <R3E/system/Config.h>

#include <common/Obstacle.h>
#include <common/ResourceManager.h>


Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, int u_id) :
    Obstacle(position, id,
             utils::j3x::get<std::string>(RM.getObjectParams("obstacles", id), "default_activation"),
             utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("obstacles", id), "default_functions"),
             utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("obstacles", id), "default_datas"), u_id)
{

}

Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, const std::string& activation,
                   const std::vector<std::string>& functions, const std::vector<std::string>& datas, int u_id) :
        Functional(activation, functions, datas, id, u_id),
        StaticObject(position,
                     {utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_x"),
                      utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_y")},
                     collision::Box(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_y"),
                                    {utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_offset_x"),
                                     utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_offset_y")}),
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

