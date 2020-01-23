//
// Created by jul3x on 26.06.2019.
//

#include <iostream>
#include <utility>

#include <engine/system/AbstractResourceManager.h>


AbstractResourceManager::AbstractResourceManager(std::string j3x_dir, std::string textures_dir,
                                                 std::string fonts_dir) : j3x_directory_{std::move(j3x_dir)},
                                                                          textures_directory_{std::move(textures_dir)},
                                                                          fonts_directory_{std::move(fonts_dir)}
{

}

utils::J3XParameters& AbstractResourceManager::getParameters(const std::string& key)
{
    auto it = parameters_.find(key);
    if (it == parameters_.end())
    {
        try
        {
            loadJ3XFile(key);

            return parameters_.at(key);
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

void AbstractResourceManager::lazyLoadTexture(const std::string& key)
{
    loadTexture(key);
}

void AbstractResourceManager::loadJ3XFile(const std::string& key)
{
    utils::J3XParameters params;

    params = utils::parse(j3x_directory_ + "/" + key + ".j3x");

    parameters_.emplace(key, params);

    std::cout << "[AbstractResourceManager] Parameters description " << key << " is loaded!" << std::endl;
}

void AbstractResourceManager::loadTexture(const std::string& key)
{
    if (!textures_[key].loadFromFile(textures_directory_ + "/"  + key + ".png"))
    {
        throw std::runtime_error("[AbstractResourceManager] " + key + " texture file not successfully loaded.");
    }

    std::cout << "[AbstractResourceManager] Texture " << key << " is loaded!" << std::endl;
}

void AbstractResourceManager::loadFont(const std::string& key)
{
    if (!fonts_[key].loadFromFile(fonts_directory_ + "/"  + key + ".ttf"))
    {
        throw std::runtime_error("[AbstractResourceManager] " + key + " font file not successfully loaded.");
    }

    std::cout << "[AbstractResourceManager] Font " << key << " is loaded!" << std::endl;
}
