//
// Created by jprolejko on 25.06.19.
//

#ifndef RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H
#define RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H

#include <list>
#include <string>
#include <map>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <game/environment/Obstacle.h>
#include <game/environment/Decoration.h>
#include <game/weapons/ShootingWeapon.h>
#include <game/weapons/Bullet.h>


// A LOT OF BOILERPLATE!
// ITS TIME TO MODIFY IT TO TEMPLATES!
// AND TO ONLY HOLD FILES - NOT THEIR INTERPRETATION

class ResourceManager {

public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& getInstance();

    BulletDescription& getBulletDescription(const std::string &key);
    ShootingWeapon& getWeapon(const std::string &key);
    sf::Texture& getTexture(const std::string &key);
    sf::Font& getFont(const std::string &key);
    sf::Font& getFont();
    std::tuple<std::list<Obstacle>, std::list<Decoration>> getMap(const std::string &key);

    // TODO LazyLoad every type of objects
    void lazyLoadTexture(const std::string &key);

private:
    ResourceManager() = default;

    void loadBulletDescription(const std::string &key);
    void loadWeapon(const std::string &key);
    void loadTexture(const std::string &key);
    void loadFont(const std::string &key);
    std::tuple<std::list<Obstacle>, std::list<Decoration>> loadMap(const std::string &key);

    std::map<std::string, sf::Texture> textures_;
    std::map<std::string, ShootingWeapon> weapons_;
    std::map<std::string, BulletDescription> bullets_;
    std::map<std::string, sf::Font> fonts_;
};


#endif //RAG3_ENGINE_SYSTEM_RESOURCEMANAGER_H
