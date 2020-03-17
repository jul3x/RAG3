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

//BulletDescription& ResourceManager::getBulletDescription(const std::string& key)
//{
//    auto it = bullets_.find(key);
//    if (it == bullets_.end())
//    {
//        try
//        {
//            loadBulletDescription(key);
//
//            return bullets_.at(key);
//        }
//        catch (std::runtime_error& e)
//        {
//            std::cerr << e.what() << std::endl;
//        }
//    }
//
//    return it->second;
//}
//
//
//ShootingWeapon& ResourceManager::getWeapon(const std::string& key)
//{
//    auto it = weapons_.find(key);
//    if (it == weapons_.end())
//    {
//        try
//        {
//            loadWeapon(key);
//
//            return weapons_.at(key);
//        }
//        catch (std::runtime_error& e)
//        {
//            std::cerr << e.what() << std::endl;
//        }
//    }
//
//    return it->second;
//}
//
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
//
//void ResourceManager::loadBulletDescription(const std::string& key)
//{
//    utils::J3XIParameters int_params;
//    utils::J3XFParameters float_params;
//    utils::J3XSParameters string_params;
//
//    std::tie(int_params, float_params, string_params) = getParameters("bullets/" + key);
//
//    bullets_.emplace(key, BulletDescription{utils::getFloat(float_params, "speed"),
//                                            utils::getFloat(float_params, "life"),
//                                            utils::getInt(int_params, "deadly_factor"),
//                                            key,
//                                            utils::getFloat(float_params, "size_x"),
//                                            utils::getFloat(float_params, "size_y"),
//                                            utils::getFloat(float_params, "burst_size")});
//
//    std::cout << "[ResourceManager] Bullet description " << key << " is loaded!" << std::endl;
//}
//
//void ResourceManager::loadWeapon(const std::string& key)
//{
//    utils::J3XIParameters int_params;
//    utils::J3XFParameters float_params;
//    utils::J3XSParameters string_params;
//
//    std::tie(int_params, float_params, string_params) = getParameters("weapons/" + key);
//
//    weapons_.emplace(key, ShootingWeapon{utils::getFloat(float_params, "bullet_timeout"),
//                                         utils::getFloat(float_params, "recoil"),
//                                         utils::getInt(int_params, "max_ammo"),
//                                         {utils::getFloat(float_params, "size_x"),
//                                          utils::getFloat(float_params, "size_y")},
//                                         {utils::getFloat(float_params, "offset_x"),
//                                          utils::getFloat(float_params, "offset_y")},
//                                         utils::getString(string_params, "bullet_type"),
//                                         utils::getInt(int_params, "bullet_quantity"),
//                                         utils::getFloat(float_params, "bullet_angular_diff"),
//                                         key});
//
//    std::cout << "[ResourceManager] Weapon " << key << " is loaded!" << std::endl;
//}


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

ResourceManager::ResourceManager() : AbstractResourceManager("../data", "../data/textures", "../data/fonts",
                                                             "../data/sounds", "../data/music")
{

}
