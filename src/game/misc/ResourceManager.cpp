//
// Created by jul3x on 26.06.2019.
//

#include <iostream>

#include <game/misc/ResourceManager.h>


ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

BulletDescription& ResourceManager::getBulletDescription(const std::string& key)
{
    auto it = bullets_.find(key);
    if (it == bullets_.end())
    {
        try
        {
            loadBulletDescription(key);

            return bullets_.at(key);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second;
}


ShootingWeapon& ResourceManager::getWeapon(const std::string& key)
{
    auto it = weapons_.find(key);
    if (it == weapons_.end())
    {
        try
        {
            loadWeapon(key);

            return weapons_.at(key);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second;
}

std::tuple<sf::Vector2i, std::vector<std::vector<bool>>, std::list<Obstacle>, std::list<Decoration>>
ResourceManager::getMap(const std::string& key)
{
    try
    {
        return loadMap(key);
    }
    catch (std::logic_error& e)
    {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

void ResourceManager::loadBulletDescription(const std::string& key)
{
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    utils::J3XSParameters string_params;

    std::tie(int_params, float_params, string_params) = getParameters("bullets/" + key);

    bullets_.emplace(key, BulletDescription{utils::getFloat(float_params, "speed"),
                                            utils::getFloat(float_params, "life"),
                                            utils::getInt(int_params, "deadly_factor"),
                                            key,
                                            utils::getFloat(float_params, "size_x"),
                                            utils::getFloat(float_params, "size_y"),
                                            utils::getFloat(float_params, "burst_size")});

    std::cout << "[ResourceManager] Bullet description " << key << " is loaded!" << std::endl;
}

void ResourceManager::loadWeapon(const std::string& key)
{
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    utils::J3XSParameters string_params;

    std::tie(int_params, float_params, string_params) = getParameters("weapons/" + key);

    weapons_.emplace(key, ShootingWeapon{utils::getFloat(float_params, "bullet_timeout"),
                                         utils::getFloat(float_params, "recoil"),
                                         utils::getInt(int_params, "max_ammo"),
                                         {utils::getFloat(float_params, "size_x"),
                                          utils::getFloat(float_params, "size_y")},
                                         {utils::getFloat(float_params, "offset_x"),
                                          utils::getFloat(float_params, "offset_y")},
                                         utils::getString(string_params, "bullet_type"),
                                         utils::getInt(int_params, "bullet_quantity"),
                                         utils::getFloat(float_params, "bullet_angular_diff"),
                                         key});

    std::cout << "[ResourceManager] Weapon " << key << " is loaded!" << std::endl;
}

std::tuple<sf::Vector2i, std::vector<std::vector<bool>>, std::list<Obstacle>, std::list<Decoration>>
ResourceManager::loadMap(const std::string& key)
{
    std::ifstream file("../data/" + key + ".j3x");
    std::vector<std::vector<bool>> blocked;
    std::list<Obstacle> obstacles;
    std::list<Decoration> decorations;
    int w, h;
    if (file)
    {
        file >> w >> h;

        blocked.resize(w);
        for (auto& row : blocked)
            row.resize(h);

        int max_number = w * h;
        int count = 0;
        short int type = 0;
        // type < 0 - decoration, type > 0 - obstacle
        while (file >> type)
        {
            blocked.at(count % w).at(count / w) = false;
            if (type < 10 && type > 0)
            {
                blocked.at(count % w).at(count / w) = true;
                obstacles.push_back({{(count % w) * Obstacle::SIZE_X_, (count / w) * Obstacle::SIZE_Y_}, type});
            }
            else if (type > -10 && type < 0)
            {
                decorations.push_back({{(count % w) * Obstacle::SIZE_X_, (count / w) * Obstacle::SIZE_Y_}, -type});
            }
            else if (type != 0)
            {
                throw std::logic_error("[ResourceManager] For now, not handled type of obstacle!");
            }
            ++count;
        }

        if (count != max_number)
        {
            throw std::logic_error("[ResourceManager] Wrong number of tiles!");
        }
    }
    else
    {
        throw std::logic_error("[ResourceManager] Map file not found! This should not happen during standard runtime.");
    }

    std::cout << "[ResourceManager] Map " << key << " is loaded!" << std::endl;

    return std::make_tuple(sf::Vector2i{w, h}, blocked, obstacles, decorations);
}

ResourceManager::ResourceManager() : AbstractResourceManager("../data/", "../data/textures/", "../data/fonts/",
                                                             "../data/sounds/", "../data/music/")
{

}
