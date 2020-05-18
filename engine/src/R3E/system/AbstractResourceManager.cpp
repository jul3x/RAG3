//
// Created by jul3x on 26.06.2019.
//

#include <iostream>
#include <utility>

#include <R3E/system/AbstractResourceManager.h>


namespace r3e {

    AbstractResourceManager::AbstractResourceManager(std::string j3x_dir, std::string textures_dir,
                                                     std::string fonts_dir, std::string sounds_dir,
                                                     std::string music_dir)
            : j3x_directory_{std::move(j3x_dir)},
              textures_directory_{std::move(textures_dir)},
              fonts_directory_{std::move(fonts_dir)},
              sounds_directory_{std::move(sounds_dir)},
              music_directory_{std::move(music_dir)}
    {
        textures_smooth_allowed_ = false;
    }

    void AbstractResourceManager::setTexturesSmoothAllowed(bool allowed)
    {
        textures_smooth_allowed_ = allowed;
    }

    utils::j3x::Parameters& AbstractResourceManager::getParameters(const std::string& key)
    {
        return getOrLoad(parameters_, std::bind(&AbstractResourceManager::loadJ3XFile, this, std::placeholders::_1), key);
    }

    sf::Texture& AbstractResourceManager::getTexture(const std::string& key)
    {
        return getOrLoad(textures_, std::bind(&AbstractResourceManager::loadTexture, this, std::placeholders::_1), key);
    }

    sf::Font& AbstractResourceManager::getFont(const std::string& key)
    {
        return getOrLoad(fonts_, std::bind(&AbstractResourceManager::loadFont, this, std::placeholders::_1), key);
    }


    sf::SoundBuffer& AbstractResourceManager::getSound(const std::string& key)
    {
        return getOrLoad(sounds_, std::bind(&AbstractResourceManager::loadSound, this, std::placeholders::_1), key);
    }

    sf::Music& AbstractResourceManager::getMusic(const std::string& key)
    {
        return getOrLoad(music_, std::bind(&AbstractResourceManager::loadMusic, this, std::placeholders::_1), key);
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
        utils::j3x::Parameters params;

        params = utils::j3x::parse(j3x_directory_ + "/" + key + ".j3x");

        parameters_.emplace(key, params);

        std::cout << "[AbstractResourceManager] Parameters description " << key << " is loaded!" << std::endl;
    }

    void AbstractResourceManager::loadTexture(const std::string& key)
    {
        if (!textures_[key].loadFromFile(textures_directory_ + "/" + key + ".png"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " texture file not successfully loaded.");
        }

        if (!textures_smooth_allowed_)
            textures_[key].setSmooth(false);

        std::cout << "[AbstractResourceManager] Texture " << key << " is loaded!" << std::endl;
    }

    void AbstractResourceManager::loadFont(const std::string& key)
    {
        if (!fonts_[key].loadFromFile(fonts_directory_ + "/" + key + ".ttf"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " font file not successfully loaded.");
        }

        std::cout << "[AbstractResourceManager] Font " << key << " is loaded!" << std::endl;
    }

    void AbstractResourceManager::loadSound(const std::string& key)
    {
        if (!sounds_[key].loadFromFile(sounds_directory_ + "/" + key + ".ogg"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " sound file not successfully loaded.");
        }

        std::cout << "[AbstractResourceManager] Sound " << key << " is loaded!" << std::endl;
    }

    void AbstractResourceManager::loadMusic(const std::string& key)
    {
        if (!music_[key].openFromFile(music_directory_ + "/" + key + ".ogg"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " music file not successfully loaded.");
        }

        std::cout << "[AbstractResourceManager] Music " << key << " is loaded!" << std::endl;
    }

} // namespace r3e