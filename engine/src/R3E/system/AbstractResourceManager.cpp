//
// Created by jul3x on 26.06.2019.
//

#include <SFML/Graphics.hpp>

#include <iostream>
#include <utility>

#include <R3E/system/AbstractResourceManager.h>
#include <R3E/system/Logger.h>


namespace r3e {

    AbstractResourceManager::AbstractResourceManager(std::string j3x_dir, std::string textures_dir,
                                                     std::string fonts_dir, std::string sounds_dir,
                                                     std::string music_dir, std::string shader_dir)
            : j3x_directory_{std::move(j3x_dir)},
              textures_directory_{std::move(textures_dir)},
              fonts_directory_{std::move(fonts_dir)},
              sounds_directory_{std::move(sounds_dir)},
              music_directory_{std::move(music_dir)},
              shader_directory_{std::move(shader_dir)}
    {
    }

    void AbstractResourceManager::setTexturesSmoothAllowed(bool allowed)
    {
        textures_smooth_allowed_ = allowed;

        for (auto& texture : textures_)
        {
            texture.second.setSmooth(textures_smooth_allowed_);
        }
    }

    void AbstractResourceManager::setFontsSmoothAllowed(bool allowed)
    {
        fonts_smooth_allowed_ = allowed;

        for (auto& font : fonts_)
        {
            font.second.setSmooth(true);
            font.second.setSmooth(fonts_smooth_allowed_);
        }
    }

    j3x::Parameters& AbstractResourceManager::getParameters(const std::string& key)
    {
        return *getOrLoad(parameters_, [this](const std::string& key) { this->loadJ3XFile(key); }, key);
    }

    sf::Texture& AbstractResourceManager::getTexture(const std::string& key)
    {
        return getOrLoad(textures_, [this](const std::string& key) { this->loadTexture(key); }, key);
    }

    sf::Font& AbstractResourceManager::getFont(const std::string& key)
    {
        return getOrLoad(fonts_, [this](const std::string& key) { this->loadFont(key); }, key);
    }


    sf::SoundBuffer& AbstractResourceManager::getSound(const std::string& key)
    {
        return getOrLoad(sounds_, [this](const std::string& key) { this->loadSound(key); }, key);
    }

    sf::Music& AbstractResourceManager::getMusic(const std::string& key)
    {
        return getOrLoad(music_, [this](const std::string& key) { this->loadMusic(key); }, key);
    }

    sf::Shader& AbstractResourceManager::getShader(const std::string& key)
    {
        return getOrLoad(shaders_, [this](const std::string& key) { this->loadShader(key); }, key);
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
        auto params = j3x::parse(j3x_directory_ + "/" + key + ".j3x");

        parameters_.emplace(key, params);

        LOG.info("[AbstractResourceManager] Parameters description " + key + " is loaded!");
    }

    void AbstractResourceManager::loadTexture(const std::string& key)
    {
        if (!textures_[key].loadFromFile(textures_directory_ + "/" + key + ".png"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " texture file not successfully loaded.");
        }

        textures_[key].setSmooth(textures_smooth_allowed_);

        LOG.info("[AbstractResourceManager] Texture " + key + " is loaded!");
    }

    void AbstractResourceManager::loadFont(const std::string& key)
    {
        if (!fonts_[key].loadFromFile(fonts_directory_ + "/" + key + ".ttf"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " font file not successfully loaded.");
        }

        fonts_[key].setSmooth(fonts_smooth_allowed_);

        LOG.info("[AbstractResourceManager] Font " + key + " is loaded!");
    }

    void AbstractResourceManager::loadSound(const std::string& key)
    {
        if (!sounds_[key].loadFromFile(sounds_directory_ + "/" + key + ".ogg"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " sound file not successfully loaded.");
        }

        LOG.info("[AbstractResourceManager] Sound " + key + " is loaded!");
    }

    void AbstractResourceManager::loadMusic(const std::string& key)
    {
        if (!music_[key].openFromFile(music_directory_ + "/" + key + ".ogg"))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " music file not successfully loaded.");
        }

        LOG.info("[AbstractResourceManager] Music " + key + " is loaded!");
    }

    void AbstractResourceManager::loadShader(const std::string& key)
    {
        if (!shaders_[key].loadFromFile(shader_directory_ + "/" + key, sf::Shader::Fragment))
        {
            throw std::runtime_error("[AbstractResourceManager] " + key + " shader file not successfully loaded.");
        }

        LOG.info("[AbstractResourceManager] Shader " + key + " is loaded!");
    }


} // namespace r3e