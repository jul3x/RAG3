//
// Created by jul3x on 25.06.19.
//

#ifndef RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H
#define RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H

#include <list>
#include <string>
#include <map>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <engine/utils/Parser.h>


class AbstractResourceManager {

public:
    AbstractResourceManager() = default;

    sf::Texture& getTexture(const std::string& key);

    sf::Font& getFont(const std::string& key);

    utils::J3XParameters& getParameters(const std::string& key);

    sf::Font& getFont();

    // TODO LazyLoad every type of objects
    void lazyLoadTexture(const std::string& key);

private:
    void loadJ3XFile(const std::string& key);

    void loadTexture(const std::string& key);

    void loadFont(const std::string& key);

    std::map<std::string, sf::Texture> textures_;
    std::map<std::string, sf::Font> fonts_;
    std::map<std::string, utils::J3XParameters> parameters_;
};


#endif //RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H
