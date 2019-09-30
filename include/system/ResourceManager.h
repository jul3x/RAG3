//
// Created by jprolejko on 25.06.19.
//

#ifndef RAG3_SYSTEM_RESOURCEMANAGER_H
#define RAG3_SYSTEM_RESOURCEMANAGER_H

#include <list>
#include <string>
#include <map>

#include <SFML/Graphics/Texture.hpp>

#include <objects/Obstacle.h>


class ResourceManager {

public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& getInstance();

    sf::Texture& getTexture(const std::string &key);
    std::list<Obstacle> getMap(const std::string &key);

    void lazyLoadTexture(const std::string &key);

private:
    ResourceManager() = default;

    void loadTexture(const std::string &key);
    std::list<Obstacle> loadMap(const std::string &key);

    std::map<std::string, sf::Texture> textures_;
};


#endif //RAG3_SYSTEM_RESOURCEMANAGER_H
