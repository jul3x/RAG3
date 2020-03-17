//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_RESOURCEMANAGER_H
#define RAG3_EDITOR_RESOURCEMANAGER_H

#include <R3E/system/AbstractResourceManager.h>
#include <editor/DecorationTile.h>
#include <editor/ObstacleTile.h>


using namespace r3e;

namespace editor {
    class ResourceManager : public AbstractResourceManager {

    public:
        ResourceManager(const ResourceManager&) = delete;

        ResourceManager& operator=(const ResourceManager&) = delete;

        static ResourceManager& getInstance();

        const std::vector<std::string>& getListOfObjects(const std::string& dir);

        const std::vector<std::string>& getFreshListOfObjects(const std::string& dir);

        const sf::Vector2f& getObjectSize(const std::string& category, const std::string& id);

        const sf::Vector2f& getObjectOffset(const std::string& category, const std::string& id);

        static std::tuple<std::list<ObstacleTile>, std::list<DecorationTile>>
        getMap(const std::string& key);

    private:
        ResourceManager();

        void loadListOfObjects(const std::string& dir);

        void loadConstraints(const std::string& key);

        std::unordered_map<std::string, std::pair<sf::Vector2f, sf::Vector2f>> objects_constraints_;

        std::unordered_map<std::string, std::vector<std::string>> list_of_objects_;
    };
} // namespace editor

#define RM ResourceManager::getInstance()

#endif // RAG3_EDITOR_RESOURCEMANAGER_H
