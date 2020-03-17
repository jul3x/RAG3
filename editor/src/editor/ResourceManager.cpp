//
// Created by jul3x on 10.03.20.
//

#include <iostream>
#include <experimental/filesystem>

#include <R3E/system/Config.h>

#include <editor/ResourceManager.h>


using namespace editor;

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

const sf::Vector2f& ResourceManager::getObjectSize(const std::string& category, const std::string& id)
{
    auto key = category + "/" + id;
    auto it = objects_constraints_.find(key);

    if (it == objects_constraints_.end())
    {
        try
        {
            loadConstraints(key);

            return objects_constraints_.at(key).first;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second.first;
}

const sf::Vector2f& ResourceManager::getObjectOffset(const std::string& category, const std::string& id)
{
    auto key = category + "/" + id;
    auto it = objects_constraints_.find(key);

    if (it == objects_constraints_.end())
    {
        try
        {
            loadConstraints(key);

            return objects_constraints_.at(key).second;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return it->second.second;
}

std::tuple<std::list<ObstacleTile>, std::list<DecorationTile>>
ResourceManager::getMap(const std::string& key)
{
    std::ifstream file(CFG.getString("paths/maps_dir") + "/" + key + ".j3x");
    std::list<ObstacleTile> obstacles_tiles;
    std::list<DecorationTile> decorations_tiles;

    int w, h;
    if (file)
    {
        file >> w >> h;

        int max_number = w * h;
        int count = 0;
        short int type = 0;

        // type < 0 - decoration, type > 0 - obstacle
        while (file >> type)
        {
            if (type < 10 && type > 0)
            {
                // TODO - different types, different blockage
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

    std::cout << "[ResourceManager] Map " << key << " is loaded!" << std::endl;

    return std::make_tuple(obstacles_tiles, decorations_tiles);
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

void ResourceManager::loadConstraints(const std::string &key)
{
    utils::J3XIParameters int_params;
    utils::J3XFParameters float_params;
    utils::J3XSParameters string_params;

    std::tie(int_params, float_params, string_params) = getParameters(key);

    objects_constraints_.emplace(key, std::make_pair<sf::Vector2f, sf::Vector2f>(
            {utils::getFloat(float_params, "size_x"), utils::getFloat(float_params, "size_y")},
            {utils::getFloat(float_params, "map_offset_x"), utils::getFloat(float_params, "map_offset_y")}));

    std::cout << "[ResourceManager] Object " << key << " constraints loaded!" << std::endl;
}

ResourceManager::ResourceManager() : AbstractResourceManager(CFG.getString("paths/j3x_dir"), CFG.getString("paths/textures_dir"), CFG.getString("paths/fonts_dir"),
                                                             CFG.getString("paths/sounds_dir"), CFG.getString("paths/music_dir"))
{

}
