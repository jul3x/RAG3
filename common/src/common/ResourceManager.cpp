//
// Created by jul3x on 10.03.20.
//

#include <iostream>
#include <experimental/filesystem>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>


ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

const utils::J3XParameters& ResourceManager::getObjectParams(const std::string& category, const std::string& id)
{
    auto key = category + "/" + id;
    auto it = objects_params_.find(key);

    if (it == objects_params_.end())
    {
        try
        {
            loadObjectParams(key);

            return objects_params_.at(key);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second;
}

std::tuple<Map::Data, Map::TileMap> ResourceManager::getMap(const std::string& key)
{
    enum class MapReading {
        None,
        TileMap,
        Characters,
        Collectibles
    };
    auto map_reading = MapReading::None;

    std::ifstream file(CFG.getString("paths/maps_dir") + "/" + key + ".j3x");
    std::list<ObstacleTile> obstacles_tiles;
    std::list<DecorationTile> decorations_tiles;
    std::list<Character> characters;
    std::list<Collectible> collectibles;
    sf::Vector2f map_size;
    std::vector<std::vector<float>> blocked;

    int w, h;
    if (file)
    {
        std::string word;

        file >> w >> h;

        blocked.resize(w);
        for (auto& row : blocked)
            row.resize(h);

        int max_number = w * h;
        int count = 0;
        int number = 0;
        int type = 0;

        std::string current_id = "";
        sf::Vector2f current_pos = {};

        // type < 0 - decoration, type > 0 - obstacle
        while (file >> word)
        {
            if (word == "tile_map:")
            {
                count = 0;
                map_reading = MapReading::TileMap;
            }
            else if (word == "characters:")
            {
                number = 0;
                map_reading = MapReading::Characters;
            }
            else if (word == "collectibles:")
            {
                number = 0;
                map_reading = MapReading::Collectibles;
            }
            else
            {
                switch (map_reading)
                {
                    case MapReading::TileMap:
                    {
                        blocked.at(count % w).at(count / w) = 0.0f;
                        type = std::stoi(word);
                        if (type < 10 && type > 0)
                        {
                            blocked.at(count % w).at(count / w) =
                                    utils::getFloat(RM.getObjectParams("obstacles_tiles", std::to_string(type)), "endurance");
                            obstacles_tiles.emplace_back(sf::Vector2f((count % w) * DecorationTile::SIZE_X_,
                                                                      (count / w) * DecorationTile::SIZE_Y_),
                                                         std::to_string(type));
                        }
                        else if (type > -10 && type < 0)
                        {
                            decorations_tiles.emplace_back(sf::Vector2f((count % w) * DecorationTile::SIZE_X_,
                                                                        (count / w) * DecorationTile::SIZE_Y_),
                                                           std::to_string(-type));
                        }
                        else if (type != 0)
                        {
                            throw std::logic_error("[ResourceManager] For now, not handled type of obstacle!");
                        }
                        ++count;
                        break;
                    }
                    case MapReading::Characters:
                    {
                        ++number;

                        if (number % 3 == 1)
                        {
                            current_id = word;
                        }
                        else if (number % 3 == 2)
                        {
                            current_pos.x = std::stof(word);
                        }
                        else
                        {
                            current_pos.y = std::stof(word);
                            characters.emplace_back(current_pos, current_id);
                        }
                        break;
                    }
                    case MapReading::Collectibles:
                    {
                        ++number;

                        if (number % 3 == 1)
                        {
                            current_id = word;
                        }
                        else if (number % 3 == 2)
                        {
                            current_pos.x = std::stof(word);
                        }
                        else
                        {
                            current_pos.y = std::stof(word);
                            collectibles.emplace_back(current_pos, current_id);
                        }
                        break;
                    }
                    default:
                    {
                        throw std::logic_error("[ResourceManager] Wrong map format!");
                    }
                }
            }
        }

        if (count != max_number)
        {
            throw std::logic_error("[ResourceManager] Wrong number of tiles!");
        }
    }
    else
    {
        throw std::logic_error("[ResourceManager] Map file not found! This should not happen during standard runtime.");
    }

    map_size = {DecorationTile::SIZE_X_ * blocked.size(), DecorationTile::SIZE_Y_ * blocked.at(0).size()};

    std::cout << "[ResourceManager] Map " << key << " is loaded!" << std::endl;

    return {{obstacles_tiles, decorations_tiles, characters, collectibles}, {map_size, blocked}};
}

bool ResourceManager::saveMap(const std::string& name, Map& map)
{
    std::ofstream file(CFG.getString("paths/maps_dir") + "/" + name + ".j3x", std::ofstream::out | std::ofstream::trunc);

    if (!file)
    {
        std::cerr << "[ResourceManager] Map saving failed";

        return false;
    }

    auto map_constraints = map.getTileConstraints();
    file << map_constraints.first.x << " " << map_constraints.first.y << std::endl;

    file << "tile_map:" << std::endl;

    std::vector<std::vector<int>> matrix;
    matrix.resize(map_constraints.first.y);

    for (auto& row : matrix)
    {
        row.resize(map_constraints.first.x, 0);
    }

    for (const auto& obstacle : map.getObstaclesTiles())
    {
        matrix.at(static_cast<size_t>((obstacle.getPosition().y - map_constraints.second.y) / DecorationTile::SIZE_Y_)).
               at(static_cast<size_t>((obstacle.getPosition().x - map_constraints.second.x) / DecorationTile::SIZE_X_)) = std::stoi(obstacle.getId());
    }

    for (const auto& decoration : map.getDecorationsTiles())
    {
        matrix.at(static_cast<size_t>((decoration.getPosition().y - map_constraints.second.y) / DecorationTile::SIZE_Y_)).
               at(static_cast<size_t>((decoration.getPosition().x - map_constraints.second.x)/ DecorationTile::SIZE_X_)) = - std::stoi(decoration.getId());
    }

    for (const auto& row : matrix)
    {
        for (const auto& item : row)
        {
            file << item << " ";
        }
        file << std::endl;
    }

    auto addObjToFile = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            file << obj.getId() << " " <<
                 obj.getPosition().x - map_constraints.second.x << " " <<
                 obj.getPosition().y - map_constraints.second.y<< std::endl;
        }
    };

    addObjToFile("characters", map.getCharacters());
    addObjToFile("collectibles", map.getCollectibles());

    std::cout << "[ResourceManager] Map file " << CFG.getString("paths/maps_dir") + "/" + name + ".j3x" << " is saved!" << std::endl;

    return true;
}

std::string ResourceManager::getConfigContent(const std::string& category, const std::string& id)
{
    std::ifstream file(CFG.getString("paths/" + category) + "/" + id + ".j3x");

    if (!file)
    {
        std::cerr << "[ResourceManager] Config file not found! This should not happen during standard runtime.";

        return "";
    }

    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::cout << "[ResourceManager] Config file " << CFG.getString("paths/" + category) + "/" + id + ".j3x" << " is loaded!" << std::endl;

    return str;
}

bool ResourceManager::saveConfigFile(const std::string& category, const std::string& id, const std::string& content)
{
    std::ofstream file(CFG.getString("paths/" + category) + "/" + id + ".j3x", std::ofstream::out | std::ofstream::trunc);

    if (!file)
    {
        std::cerr << "[ResourceManager] Config file saving failed";

        return false;
    }

    file << content;

    std::cout << "[ResourceManager] Config file " << CFG.getString("paths/" + category) + "/" + id + ".j3x" << " is saved!" << std::endl;

    return true;
}

const std::vector<std::string>& ResourceManager::getListOfObjects(const std::string& dir)
{
    auto it = list_of_objects_.find(dir);

    if (it == list_of_objects_.end())
    {
        try
        {
            loadListOfObjects(dir);

            return list_of_objects_.at(dir);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << "[ResourceManager] " << e.what() << std::endl;
        }
    }

    return it->second;
}

const std::vector<std::string>& ResourceManager::getFreshListOfObjects(const std::string& dir)
{
    static const std::vector<std::string> ERROR_OBJECT = {};

    try
    {
        loadListOfObjects(dir);
        return list_of_objects_.at(dir);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "[ResourceManager] " << e.what() << std::endl;
    }

    return ERROR_OBJECT;
}

void ResourceManager::loadListOfObjects(const std::string& dir)
{
    std::experimental::filesystem::path path{dir};

    list_of_objects_[dir].clear();
    for (const auto& file : std::experimental::filesystem::recursive_directory_iterator(path))
    {
        list_of_objects_[dir].emplace_back(file.path().filename().replace_extension().string());
    }
}

void ResourceManager::loadObjectParams(const std::string &key)
{
    objects_params_.emplace(key, getParameters(key));

    std::cout << "[ResourceManager] Object " << key << " params loaded!" << std::endl;
}

ResourceManager::ResourceManager() : AbstractResourceManager(CFG.getString("paths/j3x_dir"), CFG.getString("paths/textures_dir"), CFG.getString("paths/fonts_dir"),
                                                             CFG.getString("paths/sounds_dir"), CFG.getString("paths/music_dir"))
{

}
