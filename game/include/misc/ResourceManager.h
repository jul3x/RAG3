//
// Created by jul3x on 25.06.19.
//

#ifndef RAG3_GAME_MISC_RESOURCEMANAGER_H
#define RAG3_GAME_MISC_RESOURCEMANAGER_H

#include "../environment/Obstacle.h"
#include "../environment/Decoration.h"
#include "../weapons/ShootingWeapon.h"
#include "../weapons/Bullet.h"

#include <R3E/system/AbstractResourceManager.h>


using namespace r3e;

class ResourceManager : public AbstractResourceManager {

public:
    ResourceManager(const ResourceManager&) = delete;

    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& getInstance();

    BulletDescription& getBulletDescription(const std::string& key);

    ShootingWeapon& getWeapon(const std::string& key);

    static std::tuple<sf::Vector2i, std::vector<std::vector<float>>, std::list<Obstacle>, std::list<Decoration>>
    getMap(const std::string& key);

    // TODO LazyLoad every type of objects

private:
    ResourceManager();

    void loadBulletDescription(const std::string& key);

    void loadWeapon(const std::string& key);

    static std::tuple<sf::Vector2i, std::vector<std::vector<float>>, std::list<Obstacle>, std::list<Decoration>>
    loadMap(const std::string& key);

    std::unordered_map<std::string, ShootingWeapon> weapons_;
    std::unordered_map<std::string, BulletDescription> bullets_;
};

#define RM ResourceManager::getInstance()

#endif //RAG3_GAME_MISC_RESOURCEMANAGER_H
