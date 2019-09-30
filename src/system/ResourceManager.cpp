//
// Created by jprolejko on 26.06.2019.
//

#include <iostream>

#include <system/ResourceManager.h>


ResourceManager& ResourceManager::getInstance() {
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
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

void ResourceManager::lazyLoadTexture(const std::string &key) {
    loadTexture(key);
}

void ResourceManager::loadTexture(const std::string &key) {
    if (!textures_[key].loadFromFile("data/textures/" + key + ".png"))
    {
        throw std::runtime_error("[ResourceManager] " + key + " texture file not successfully loaded.");
    }
}