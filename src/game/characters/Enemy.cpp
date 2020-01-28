//
// Created by jul3x on 31.10.19.
//

#include <game/misc/ResourceManager.h>
#include <engine/utils/Geometry.h>
#include <engine/utils/AStar.h>
#include <game/characters/Enemy.h>
#include <game/Game.h>
#include <game/weapons/NoWeapon.h>


Enemy::Enemy(const sf::Vector2f& position,
             const sf::Vector2f& velocity) :
        Character(position, velocity, 10)
{
    weapons_in_backpack_.push_back(
            std::make_unique<ShootingWeapon>(ResourceManager::getInstance().getWeapon("desert_eagle")));

    current_weapon_ = weapons_in_backpack_.begin();
}

bool Enemy::update(float time_elapsed)
{
    bool is_alive = Character::update(time_elapsed);
    float rotation;
    std::tie(std::ignore, rotation) =
            utils::geo::cartesianToPolar(Game::get().getPlayerPosition() - this->getPosition());

    this->setRotation(rotation * 180.0f / static_cast<float>(M_PI));

    int noise = utils::num::getRandom(-10, 10);
    //this->setVelocity(3 * std::cos(rotation) - noise * std::sin(rotation), 3 * std::sin(rotation) + noise * std::cos(rotation));
  //  shot();

    auto &grid = Game::get().getMapBlockage();

    int start_x = std::round(this->getPosition().x / Obstacle::SIZE_X_);
    int start_y = std::round(this->getPosition().y / Obstacle::SIZE_Y_);

    int goal_x = std::round(Game::get().getPlayerPosition().x / Obstacle::SIZE_X_);
    int goal_y = std::round(Game::get().getPlayerPosition().y / Obstacle::SIZE_Y_);

    //std::cout << start_x << " " << start_y << ", " << goal_x << " " << goal_y << std::endl;
    // TODO - Implement AgentsManager and Agent interface
    //if (path_.empty())
    path_ = utils::astar::astar(grid, {start_x, start_y}, {goal_x, goal_y});
    //std::cout << path.size() << std::endl;
   // static int i = 0;
    if (!path_.empty())
    {

        this->setVelocity((path_.at(0).first - start_x), (path_.at(0).second - start_y));
        //if (utils::num::isNearlyEqual({path_.at(0).first, path_.at(0).second}, {start_x, start_y}, 0.01f)) ++i;
    }

    return is_alive;
}
