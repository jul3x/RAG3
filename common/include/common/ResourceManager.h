//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_RESOURCEMANAGER_H
#define RAG3_COMMON_INCLUDE_COMMON_RESOURCEMANAGER_H

#include <R3E/system/AbstractResourceManager.h>

#include <common/DecorationTile.h>
#include <common/ObstacleTile.h>
#include <common/Map.h>


using namespace r3e;

class ResourceManager : public AbstractResourceManager {

public:
    ResourceManager(const ResourceManager&) = delete;

    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& getInstance();

    std::vector<std::string>& getListOfObjects(const std::string& dir);
    std::vector<std::string>& getFreshListOfObjects(const std::string& dir);
    const utils::j3x::Parameters& getObjectParams(const std::string& category, const std::string& id);
    static std::tuple<Map::Data, Map::TileMap> getMap(const std::string& key);
    static std::string getConfigContent(const std::string& category, const std::string& id);

    static bool saveMap(const std::string& name, Map& map);
    static bool saveConfigFile(const std::string& category, const std::string& id, const std::string& content);

private:
    ResourceManager();

    void loadListOfObjects(const std::string& dir);

    std::unordered_map<std::string, std::vector<std::string>> list_of_objects_;
};

#define RM ResourceManager::getInstance()

#endif //RAG3_COMMON_INCLUDE_COMMON_RESOURCEMANAGER_H
