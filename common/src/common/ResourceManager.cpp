//
// Created by jul3x on 10.03.20.
//

#include <iostream>
#include <experimental/filesystem>

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>
#include <R3E/j3x/J3X.h>

#include <common/ResourceManager.h>


ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

const j3x::Parameters& ResourceManager::getObjectParams(const std::string& category, const std::string& id)
{
    return getParameters(category + "/" + id);
}

std::tuple<Map::Data, Map::TileMap>& ResourceManager::getMap(const std::string& key)
{
    static std::tuple<Map::Data, Map::TileMap> map;
    auto map_description = j3x::parse(CFG.get<std::string>("paths/maps_dir") + "/" + key + ".j3x");
    auto get_param = [&map_description](const std::string& param) -> const j3x::List& {
        return j3x::get<j3x::List>(*map_description, param);
    };

    auto& obstacles_tiles = std::get<0>(std::get<0>(map));
    auto& decorations_tiles = std::get<1>(std::get<0>(map));
    auto& characters = std::get<2>(std::get<0>(map));
    auto& specials = std::get<3>(std::get<0>(map));
    auto& obstacles = std::get<4>(std::get<0>(map));
    auto& decorations = std::get<5>(std::get<0>(map));
    auto& weapons = std::get<6>(std::get<0>(map));

    auto map_size = static_cast<sf::Vector2i>(j3x::get<sf::Vector2f>(*map_description, "map_size"));
    if (map_size.x <= 0 || map_size.y <= 0)
        throw std::invalid_argument("[ResourceManager] Map size should be positive!");

    auto& blocked = std::get<1>(std::get<1>(map));
    blocked.resize(map_size.x);
    for (auto& column : blocked)
        column.resize(map_size.y);
    std::get<0>(std::get<1>(map)) = {DecorationTile::SIZE_X_ * blocked.size(),
                                           DecorationTile::SIZE_Y_ * blocked.at(0).size()};

    auto tile_number = 0;
    for (auto& tile : get_param( "tile_map"))
    {
        auto tile_id = j3x::getObj<int>(tile);
        auto column = tile_number % map_size.x;
        auto row = tile_number / map_size.x;
        blocked.at(column).at(row) = 0.0f;
        if (tile_id > 0)
        {
            blocked.at(tile_number % map_size.x).at(tile_number / map_size.x) = 9999999.0f;
            obstacles_tiles.emplace_back(std::make_shared<ObstacleTile>(
                    sf::Vector2f{column * DecorationTile::SIZE_X_, row * DecorationTile::SIZE_Y_},
                    std::to_string(tile_id)));
        }
        else if (tile_id < 0)
        {
            decorations_tiles.emplace_back(std::make_shared<DecorationTile>(
                    sf::Vector2f{column * DecorationTile::SIZE_X_, row * DecorationTile::SIZE_Y_},
                    std::to_string(-tile_id)));
        }
        ++tile_number;

        if (tile_number >= map_size.x * map_size.y)
            break;
    }

    if (tile_number < map_size.x * map_size.y)
        throw std::invalid_argument("[ResourceManager] Wrong size of map!");

    try
    {
        for (size_t i = 0; i < get_param("decorations_pos").size(); ++i)
        {
            decorations.emplace_back(std::make_shared<Decoration>(
                    j3x::getObj<sf::Vector2f>(get_param("decorations_pos"), i),
                    j3x::getObj<std::string>(get_param("decorations_id"), i),
                    j3x::getObj<int>(get_param("decorations_uid"), i)));
        }

        for (size_t i = 0; i < get_param("weapons_id").size(); ++i)
        {
            weapons.emplace_back(std::make_shared<PlacedWeapon>(
                    j3x::getObj<sf::Vector2f>(get_param("weapons_pos"), i),
                    j3x::getObj<float>(get_param("weapons_dir"), i),
                    j3x::getObj<std::string>(get_param("weapons_id"), i),
                    j3x::getObj<std::string>(get_param("weapons_usage"), i),
                    j3x::getObj<float>(get_param("weapons_usage_data"), i),
                    j3x::getObj<int>(get_param("weapons_uid"), i)));
        }

        for (size_t i = 0; i < get_param("obstacles_id").size(); ++i)
        {
            auto& id = j3x::getObj<std::string>(get_param("obstacles_id"), i);
            auto& pos = j3x::getObj<sf::Vector2f>(get_param("obstacles_pos"), i);
            auto blocked_pos = pos + j3x::get<sf::Vector2f>(RM.getObjectParams("obstacles", id), "collision_offset");
            obstacles.emplace_back(std::make_shared<Obstacle>(
                    pos, id,
                    j3x::getObj<std::string>(get_param("obstacles_activation"), i),
                    j3x::getObj<j3x::List>(get_param("obstacles_funcs"), i),
                    j3x::getObj<j3x::List>(get_param("obstacles_datas"), i),
                    j3x::getObj<int>(get_param("obstacles_uid"), i)));

            blocked.at(std::round(blocked_pos.x / DecorationTile::SIZE_X_)).
                    at(std::round(blocked_pos.y / DecorationTile::SIZE_Y_)) =
                    j3x::get<float>(RM.getObjectParams("obstacles", id), "endurance");
        }

        for (size_t i = 0; i < get_param("characters_id").size(); ++i)
        {
            characters.emplace_back(std::make_shared<NPC>(
                    j3x::getObj<sf::Vector2f>(get_param("characters_pos"), i),
                    j3x::getObj<std::string>(get_param("characters_id"), i),
                    j3x::getObj<std::string>(get_param("characters_activation"), i),
                    j3x::getObj<j3x::List>(get_param("characters_funcs"), i),
                    j3x::getObj<j3x::List>(get_param("characters_datas"), i),
                    j3x::getObj<int>(get_param("characters_uid"), i)));
            characters.back()->setTalkScenario(j3x::getObj<j3x::List>(get_param("characters_talks"), i));
        }

        for (size_t i = 0; i < get_param("specials_id").size(); ++i)
        {
            specials.emplace_back(std::make_shared<Special>(
                    j3x::getObj<sf::Vector2f>(get_param("specials_pos"), i),
                    j3x::getObj<std::string>(get_param("specials_id"), i),
                    j3x::getObj<std::string>(get_param("specials_activation"), i),
                    j3x::getObj<j3x::List>(get_param("specials_funcs"), i),
                    j3x::getObj<j3x::List>(get_param("specials_datas"), i),
                    j3x::getObj<bool>(get_param("specials_is_active"), i),
                    j3x::getObj<int>(get_param("specials_uid"), i)));
        }
    }
    catch (const std::out_of_range& e)
    {
        throw std::out_of_range("[ResourceManager] Wrong size of lists with elements in map description");
    }

    auto set_random_initial_frame = [](auto& objects)
    {
        for (auto& object : objects)
        {
            auto number = object->getFramesNumber();
            if (number <= 1) continue;

            object->setCurrentFrame(utils::num::getRandom<int>(0, number - 1));
        }
    };

    set_random_initial_frame(obstacles_tiles);
    set_random_initial_frame(decorations_tiles);
    set_random_initial_frame(characters);
    set_random_initial_frame(specials);
    set_random_initial_frame(obstacles);
    set_random_initial_frame(decorations);
    set_random_initial_frame(weapons);

    std::cout << "[ResourceManager] Map " << key << " is loaded!" << std::endl;

    return map;
}

bool ResourceManager::saveMap(const std::string& name, Map& map)
{
    std::ofstream file(CFG.get<std::string>("paths/maps_dir") + "/" + name + ".j3x", std::ofstream::out | std::ofstream::trunc);

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

    for (const auto& obstacle : map.getList<ObstacleTile>())
    {
        matrix.at(static_cast<size_t>((obstacle->getPosition().y - map_constraints.second.y) / DecorationTile::SIZE_Y_)).
               at(static_cast<size_t>((obstacle->getPosition().x - map_constraints.second.x) / DecorationTile::SIZE_X_)) = std::stoi(obstacle->getId());
    }

    for (const auto& decoration : map.getList<DecorationTile>())
    {
        matrix.at(static_cast<size_t>((decoration->getPosition().y - map_constraints.second.y) / DecorationTile::SIZE_Y_)).
               at(static_cast<size_t>((decoration->getPosition().x - map_constraints.second.x) / DecorationTile::SIZE_X_)) = - std::stoi(decoration->getId());
    }

    for (const auto& row : matrix)
    {
        for (const auto& item : row)
        {
            file << item << " ";
        }
        file << std::endl;
    }

    auto add_obj_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getUniqueId() << std::endl;
        }
    };

    auto add_weapon_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getRotation() << " " << obj->getUniqueId() << " " <<
                 obj->getUsageStr() << " " << obj->getData() << std::endl;
        }
    };

    auto add_obstacle_obj_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            auto new_data = obj->getDatasStr();
            std::replace(new_data.begin(), new_data.end(), ' ', '_');

            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getUniqueId() << " " <<
                 obj->getActivation() << " " <<
                 "[" << obj->getFunctionsStr() << "] [" <<
                 new_data << "]" << std::endl;
        }
    };

    auto add_special_obj_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            auto new_data = obj->getDatasStr();
            std::replace(new_data.begin(), new_data.end(), ' ', '_');

            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getUniqueId() << " " <<
                 obj->isActive() << " " <<
                 obj->getActivation() << " " <<
                 "[" << obj->getFunctionsStr() << "] [" <<
                 new_data << "]" << std::endl;
        }
    };

    auto add_character_obj_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            auto new_data = obj->getDatasStr();
            std::replace(new_data.begin(), new_data.end(), ' ', '_');

            auto new_conv = obj->getTalkScenarioStr();
            std::replace(new_conv.begin(), new_conv.end(), ' ', '_');


            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getUniqueId() << " " <<
                 obj->getActivation() << " " <<
                 "[" << obj->getFunctionsStr() << "] [" << new_data << "]" <<
                 " [" << new_conv << "]" << std::endl;
        }
    };

    add_obj_to_file("decorations", map.getList<Decoration>());
    add_obstacle_obj_to_file("obstacles", map.getList<Obstacle>());
    add_character_obj_to_file("characters", map.getList<NPC>());
    add_special_obj_to_file("specials", map.getList<Special>());
    add_weapon_to_file("weapons", map.getList<PlacedWeapon>());

    std::cout << "[ResourceManager] Map file " << CFG.get<std::string>("paths/maps_dir") + "/" + name + ".j3x" << " is saved!" << std::endl;

    return true;
}

std::string ResourceManager::getConfigContent(const std::string& category, const std::string& id)
{
    std::ifstream file(CFG.get<std::string>("paths/" + category) + "/" + id + ".j3x");

    if (!file)
    {
        std::cerr << "[ResourceManager] Config file not found! This should not happen during standard runtime.";

        return "";
    }

    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::cout << "[ResourceManager] Config file " << CFG.get<std::string>("paths/" + category) + "/" + id + ".j3x" << " is loaded!" << std::endl;

    return str;
}

bool ResourceManager::saveConfigFile(const std::string& category, const std::string& id, const std::string& content)
{
    std::ofstream file(CFG.get<std::string>("paths/" + category) + "/" + id + ".j3x", std::ofstream::out | std::ofstream::trunc);

    if (!file)
    {
        std::cerr << "[ResourceManager] Config file saving failed";

        return false;
    }

    file << content;

    std::cout << "[ResourceManager] Config file " << CFG.get<std::string>("paths/" + category) + "/" + id + ".j3x" << " is saved!" << std::endl;

    return true;
}

std::vector<std::string>& ResourceManager::getListOfObjects(const std::string& dir)
{
    return getOrLoad(list_of_objects_, std::bind(&ResourceManager::loadListOfObjects, this, std::placeholders::_1), dir);
}

std::vector<std::string>& ResourceManager::getFreshListOfObjects(const std::string& dir)
{
    static std::vector<std::string> ERROR_OBJECT = {};

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

ResourceManager::ResourceManager() : AbstractResourceManager(CFG.get<std::string>("paths/j3x_dir"), CFG.get<std::string>("paths/textures_dir"), CFG.get<std::string>("paths/fonts_dir"),
                                                             CFG.get<std::string>("paths/sounds_dir"), CFG.get<std::string>("paths/music_dir"))
{

}
