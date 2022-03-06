//
// Created by jul3x on 25.06.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTRESOURCEMANAGER_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTRESOURCEMANAGER_H

#include <list>
#include <string>
#include <functional>
#include <unordered_map>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include <R3E/j3x/J3X.h>
#include <R3E/system/Logger.h>


namespace r3e {

    class AbstractResourceManager {

    public:
        AbstractResourceManager() = default;

        AbstractResourceManager(std::string j3x_dir, std::string textures_dir, std::string fonts_dir,
                                std::string sounds_dir, std::string music_dir, std::string shader_dir);

        void setTexturesSmoothAllowed(bool allowed);
        void setFontsSmoothAllowed(bool allowed);

        sf::Texture& getTexture(const std::string& key);
        sf::SoundBuffer& getSound(const std::string& key);
        sf::Music& getMusic(const std::string& key);
        sf::Font& getFont(const std::string& key);
        j3x::Parameters& getParameters(const std::string& key);
        sf::Shader& getShader(const std::string& key);
        sf::Font& getFont();

        // TODO LazyLoad every type of objects
        // TODO - templates
        void lazyLoadTexture(const std::string& key);

        static bool fileExists(const std::string& filename);
        static void copyFile(const std::string& in, const std::string& out);
        static void copyFileIfNotExist(const std::string& in, const std::string& out);

    protected:
        template<class T>
        T& getOrLoad(std::unordered_map<std::string, T>& objs,
                     const std::function<void(const std::string&)>& func,
                     const std::string& key)
        {
            auto it = objs.find(key);
            if (it == objs.end())
            {
                try
                {
                    func(key);

                    return objs.at(key);
                }
                catch (std::runtime_error& e)
                {
                    LOG.error(e.what());
                }
            }

            return it->second;
        }

        std::unordered_map<std::string, std::shared_ptr<j3x::Parameters>> parameters_;

    private:
        void loadJ3XFile(const std::string& key);
        void loadTexture(const std::string& key);
        void loadSound(const std::string& key);
        void loadMusic(const std::string& key);
        void loadFont(const std::string& key);
        void loadShader(const std::string& key);

        std::unordered_map<std::string, sf::Texture> textures_;
        std::unordered_map<std::string, sf::SoundBuffer> sounds_;
        std::unordered_map<std::string, sf::Music> music_;
        std::unordered_map<std::string, sf::Font> fonts_;
        std::unordered_map<std::string, sf::Shader> shaders_;

        std::string j3x_directory_;
        std::string textures_directory_;
        std::string fonts_directory_;
        std::string sounds_directory_;
        std::string music_directory_;
        std::string shader_directory_;

        bool textures_smooth_allowed_{false};
        bool fonts_smooth_allowed_{true};

    };

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_ABSTRACTRESOURCEMANAGER_H
