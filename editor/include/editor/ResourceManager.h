//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_RESOURCEMANAGER_H
#define RAG3_EDITOR_RESOURCEMANAGER_H

#include <R3E/system/AbstractResourceManager.h>


using namespace r3e;

namespace editor {
    class ResourceManager : public AbstractResourceManager {

    public:
        ResourceManager(const ResourceManager&) = delete;

        ResourceManager& operator=(const ResourceManager&) = delete;

        static ResourceManager& getInstance();

        const std::vector<std::string>& getListOfObjects(const std::string& dir);

    //    BulletDescription& getBulletDescription(const std::string& key);
    //
    //    ShootingWeapon& getWeapon(const std::string& key);
    //
    //    static std::tuple<sf::Vector2i, std::vector<std::vector<float>>, std::list<Obstacle>, std::list<Decoration>>
    //    getMap(const std::string& key);

    private:
        ResourceManager();

        void loadListOfObjects(const std::string& dir);
    //    void loadBulletDescription(const std::string& key);
    //
    //    void loadWeapon(const std::string& key);
    //
    //    static std::tuple<sf::Vector2i, std::vector<std::vector<float>>, std::list<Obstacle>, std::list<Decoration>>
    //    loadMap(const std::string& key);
    //
    //    std::unordered_map<std::string, ShootingWeapon> weapons_;
    //    std::unordered_map<std::string, BulletDescription> bullets_;

        std::unordered_map<std::string, std::vector<std::string>> list_of_objects_;
    };
} // namespace editor

#define RM ResourceManager::getInstance()

#endif // RAG3_EDITOR_RESOURCEMANAGER_H
