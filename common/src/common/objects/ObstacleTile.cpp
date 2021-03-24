//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>

#include <common/objects/DecorationTile.h>
#include <common/objects/ObstacleTile.h>
#include <common/ResourceManager.h>


ObstacleTile::ObstacleTile(const sf::Vector2f& position, const std::string& id) :
        Identifiable(id),
        StaticObject(position,
                     {DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_},
                     collision::Box(
                             RMGET<sf::Vector2f>("obstacles_tiles", id, "collision_size").x,
                             RMGET<sf::Vector2f>("obstacles_tiles", id, "collision_size").y,
                             RMGET<sf::Vector2f>("obstacles_tiles", id, "collision_offset")),
                     &RM.getTexture("obstacles_tiles/" + id),
                     RMGET<int>("obstacles_tiles", id, "z_index"),
                     0, 0.0f)
{
    this->changeOrigin(sf::Vector2f(DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_) / 2.0f);
}
