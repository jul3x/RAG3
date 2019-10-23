//
// Created by jprolejko on 26.06.2019.
//

#include <iostream>
#include <fstream>

#include <utils/Parser.h>

#include <system/ResourceManager.h>


ResourceManager& ResourceManager::getInstance() {
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

Weapon& ResourceManager::getWeapon(const std::string &key) {
    if (weapons_.find(key) == weapons_.end())
    {
        try
        {
            loadWeapon(key);
            std::cout << "[ResourceManager] Weapon " << key << " is loaded!" << std::endl;
        }
        catch (std::runtime_error &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return weapons_.at(key);
}

sf::Texture& ResourceManager::getTexture(const std::string &key) {
    if (textures_.find(key) == textures_.end())
    {
        try
        {
            loadTexture(key);
            std::cout << "[ResourceManager] Texture " << key << " is loaded!" << std::endl;
        }
        catch (std::runtime_error &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return textures_.at(key);
}

std::tuple<std::list<Obstacle>, std::list<Decoration>> ResourceManager::getMap(const std::string &key) {
    try
    {
        return loadMap(key);
    }
    catch (std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void ResourceManager::lazyLoadTexture(const std::string &key) {
    loadTexture(key);
}

void ResourceManager::loadWeapon(const std::string &key) {
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    std::tie(int_params, float_params) = utils::parse("data/weapons/" + key + ".j3x");

    weapons_.emplace(key, Weapon{utils::getFloat(float_params, "bullet_timeout"),
                           utils::getFloat(float_params, "recoil"),
                           utils::getInt(int_params, "max_ammo"),
                           {utils::getFloat(float_params, "size_x"),
                            utils::getFloat(float_params, "size_y")},
                           {utils::getFloat(float_params, "offset_x"),
                            utils::getFloat(float_params, "offset_y")},
                           key});
}

void ResourceManager::loadTexture(const std::string &key) {
    if (!textures_[key].loadFromFile("data/textures/" + key + ".png"))
    {
        throw std::runtime_error("[ResourceManager] " + key + " texture file not successfully loaded.");
    }
}

std::tuple<std::list<Obstacle>, std::list<Decoration>> ResourceManager::loadMap(const std::string &key) {
    std::ifstream file("data/" + key + ".j3x");
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

    return std::make_tuple(obstacles, decorations);
}