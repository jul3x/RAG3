//
// Created by jul3x on 26.06.2019.
//

#include <iostream>

#include <engine/utils/Parser.h>
#include <engine/system/AbstractResourceManager.h>


AbstractResourceManager& AbstractResourceManager::getInstance()
{
    static AbstractResourceManager resource_manager_instance;
    return resource_manager_instance;
}

BulletDescription& AbstractResourceManager::getBulletDescription(const std::string& key)
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


ShootingWeapon& AbstractResourceManager::getWeapon(const std::string& key)
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

sf::Texture& AbstractResourceManager::getTexture(const std::string& key)
{
    auto it = textures_.find(key);
    if (it == textures_.end())
    {
        try
        {
            loadTexture(key);

            return textures_.at(key);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second;
}

sf::Font& AbstractResourceManager::getFont(const std::string& key)
{
    auto it = fonts_.find(key);
    if (it == fonts_.end())
    {
        try
        {
            loadFont(key);

            return fonts_.at(key);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second;
}

sf::Font& AbstractResourceManager::getFont()
{
    return this->getFont("default");
}

std::tuple<std::list<Obstacle>, std::list<Decoration>> AbstractResourceManager::getMap(const std::string& key)
{
    try
    {
        return loadMap(key);
    }
    catch (std::logic_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void AbstractResourceManager::lazyLoadTexture(const std::string& key)
{
    loadTexture(key);
}

void AbstractResourceManager::loadBulletDescription(const std::string& key)
{
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    utils::J3XSParameters string_params;

    std::tie(int_params, float_params, string_params) = utils::parse("../data/bullets/" + key + ".j3x");

    bullets_.emplace(key, BulletDescription{utils::getFloat(float_params, "speed"),
                                            utils::getFloat(float_params, "life"),
                                            utils::getInt(int_params, "deadly_factor"),
                                            key,
                                            utils::getFloat(float_params, "size_x"),
                                            utils::getFloat(float_params, "size_y")});

    std::cout << "[AbstractResourceManager] Bullet description " << key << " is loaded!" << std::endl;
}

void AbstractResourceManager::loadWeapon(const std::string& key)
{
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    utils::J3XSParameters string_params;

    std::tie(int_params, float_params, string_params) = utils::parse("../data/weapons/" + key + ".j3x");

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

    std::cout << "[AbstractResourceManager] Weapon " << key << " is loaded!" << std::endl;
}

void AbstractResourceManager::loadTexture(const std::string& key)
{
    if (!textures_[key].loadFromFile("../data/textures/" + key + ".png"))
    {
        throw std::runtime_error("[AbstractResourceManager] " + key + " texture file not successfully loaded.");
    }

    std::cout << "[AbstractResourceManager] Texture " << key << " is loaded!" << std::endl;
}

void AbstractResourceManager::loadFont(const std::string& key)
{
    if (!fonts_[key].loadFromFile("../data/fonts/" + key + ".ttf"))
    {
        throw std::runtime_error("[AbstractResourceManager] " + key + " font file not successfully loaded.");
    }

    std::cout << "[AbstractResourceManager] Font " << key << " is loaded!" << std::endl;
}

std::tuple<std::list<Obstacle>, std::list<Decoration>> AbstractResourceManager::loadMap(const std::string& key)
{
    std::ifstream file("../data/" + key + ".j3x");
    std::list<Obstacle> obstacles;
    std::list<Decoration> decorations;

    if (file)
    {
        int w, h;
        file >> w >> h;

        int max_number = w * h;
        int count = 0;
        short int type = 0;
        // type < 0 - decoration, type > 0 - obstacle
        while (file >> type)
        {
            if (type < 10 && type > 0)
            {
                obstacles.push_back({{(count % w) * Obstacle::SIZE_X_, (count / w) * Obstacle::SIZE_Y_}, type});
            }
            else if (type > -10 && type < 0)
            {
                decorations.push_back({{(count % w) * Obstacle::SIZE_X_, (count / w) * Obstacle::SIZE_Y_}, -type});
            }
            else if (type != 0)
            {
                throw std::logic_error("[AbstractResourceManager] For now, not handled type of obstacle!");
            }
            ++count;
        }

        if (count != max_number)
        {
            throw std::logic_error("[AbstractResourceManager] Wrong number of tiles!");
        }
    }
    else
    {
        throw std::logic_error("[AbstractResourceManager] Map file not found! This should not happen during standard runtime.");
    }

    std::cout << "[AbstractResourceManager] Map " << key << " is loaded!" << std::endl;

    return std::make_tuple(obstacles, decorations);
}