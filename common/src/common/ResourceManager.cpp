//
// Created by jul3x on 10.03.20.
//

#include <iostream>
#include <experimental/filesystem>

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/ResourceManager.h>



ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager resource_manager_instance;
    return resource_manager_instance;
}

const utils::j3x::Parameters& ResourceManager::getObjectParams(const std::string& category, const std::string& id)
{
    return getParameters(category + "/" + id);
}

std::tuple<Map::Data, Map::TileMap> ResourceManager::getMap(const std::string& key)
{
    enum class MapReading {
        None,
        TileMap,
        Characters,
        Specials,
        Obstacles,
        Decorations,
        Weapons
    };
    auto map_reading = MapReading::None;

    std::ifstream file(CFG.get<std::string>("paths/maps_dir") + "/" + key + ".j3x");
    std::list<std::shared_ptr<ObstacleTile>> obstacles_tiles;
    std::list<std::shared_ptr<DecorationTile>> decorations_tiles;
    std::list<std::shared_ptr<NPC>> characters;
    std::list<std::shared_ptr<Special>> specials;
    std::list<std::shared_ptr<Obstacle>> obstacles;
    std::list<std::shared_ptr<Decoration>> decorations;
    std::list<std::shared_ptr<PlacedWeapon>> weapons;
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
        bool should_add_new_object = false;

        std::string current_id = "";
        sf::Vector2f current_pos = {};
        float direction = 0.0f;
        int u_id = -1;

        std::string activation = "";
        std::vector<std::string> functions;
        std::vector<std::string> f_datas;

        std::string usage;
        float usage_data;

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
            else if (word == "specials:")
            {
                number = 0;
                map_reading = MapReading::Specials;
            }
            else if (word == "obstacles:")
            {
                number = 0;
                map_reading = MapReading::Obstacles;
            }
            else if (word == "decorations:")
            {
                number = 0;
                map_reading = MapReading::Decorations;
            }
            else if (word == "weapons:")
            {
                number = 0;
                map_reading = MapReading::Weapons;
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
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles_tiles", std::to_string(type)), "endurance");
                            obstacles_tiles.emplace_back(std::make_shared<ObstacleTile>(
                                    sf::Vector2f((count % w) * DecorationTile::SIZE_X_,
                                                 (count / w) * DecorationTile::SIZE_Y_),
                                    std::to_string(type)));
                        }
                        else if (type > -10 && type < 0)
                        {
                            decorations_tiles.emplace_back(std::make_shared<DecorationTile>(
                                    sf::Vector2f((count % w) * DecorationTile::SIZE_X_,
                                                 (count / w) * DecorationTile::SIZE_Y_),
                                    std::to_string(-type)));
                        }
                        else if (type != 0)
                        {
                            throw std::logic_error("[ResourceManager] For now, not handled type of obstacle!");
                        }
                        ++count;
                        break;
                    }
                    case MapReading::Decorations:
                    {
                        ++number;
                        should_add_new_object = false;

                        if (number % 4 == 1)
                        {
                            current_id = word;
                        }
                        else if (number % 4 == 2)
                        {
                            current_pos.x = std::stof(word);
                        }
                        else if (number % 4 == 3)
                        {
                            current_pos.y = std::stof(word);
                        }
                        else
                        {
                            u_id = std::stoi(word);
                            should_add_new_object = true;
                        }
                        break;
                    }
                    case MapReading::Weapons:
                    {
                        ++number;
                        should_add_new_object = false;

                        if (number % 7 == 1)
                        {
                            current_id = word;
                        }
                        else if (number % 7 == 2)
                        {
                            current_pos.x = std::stof(word);
                        }
                        else if (number % 7 == 3)
                        {
                            current_pos.y = std::stof(word);
                        }
                        else if (number % 7 == 4)
                        {
                            direction = std::stof(word);
                        }
                        else if (number % 7 == 5)
                        {
                            u_id = std::stoi(word);
                        }
                        else if (number % 7 == 6)
                        {
                            usage = word;
                        }
                        else
                        {
                            usage_data = std::stof(word);
                            should_add_new_object = true;
                        }

                        break;
                    }
                    case MapReading::Obstacles:
                    case MapReading::Specials:
                    case MapReading::Characters:
                    {
                        ++number;
                        should_add_new_object = false;

                        if (number % 7 == 1)
                        {
                            current_id = word;
                        }
                        else if (number % 7 == 2)
                        {
                            current_pos.x = std::stof(word);
                        }
                        else if (number % 7 == 3)
                        {
                            current_pos.y = std::stof(word);
                        }
                        else if (number % 7 == 4)
                        {
                            u_id = std::stoi(word);
                        }
                        else if (number % 7 == 5)
                        {
                            activation = word;
                        }
                        else if (number % 7 == 6)
                        {
                            if (word.length() < 2)
                                throw std::logic_error("[ResourceManager] Wrong map list object format!");

                            utils::j3x::tokenize(word.substr(1, word.length() - 2), utils::j3x::DELIMITER_, functions);
                        }
                        else
                        {
                            std::replace(word.begin(), word.end(), '_', ' ');

                            if (word.length() < 2)
                                throw std::logic_error("[ResourceManager] Wrong map list object format!");

                            utils::j3x::tokenize(word.substr(1, word.length() - 2), utils::j3x::DELIMITER_, f_datas);
                            should_add_new_object = true;
                        }

                        break;
                    }
                    default:
                    {
                        throw std::logic_error("[ResourceManager] Wrong map format!");
                    }
                }

                if (should_add_new_object)
                {
                    switch (map_reading)
                    {
                        case MapReading::Characters:
                            if (functions.size() != f_datas.size())
                            {
                                throw std::logic_error("[ResourceManager] Wrong map special object data and function format!");
                            }

                            characters.emplace_back(std::make_shared<NPC>(current_pos, current_id,
                                                                          activation, functions, f_datas, u_id));
                            break;
                        case MapReading::Specials:
                            if (functions.size() != f_datas.size())
                            {
                                throw std::logic_error("[ResourceManager] Wrong map special object data and function format!");
                            }

                            specials.emplace_back(std::make_shared<Special>(current_pos, current_id,
                                                                            activation, functions, f_datas, u_id));
                            break;
                        case MapReading::Obstacles:
                            if (functions.size() != f_datas.size())
                            {
                                throw std::logic_error("[ResourceManager] Wrong map special object data and function format!");
                            }

                            obstacles.emplace_back(std::make_shared<Obstacle>(current_pos, current_id,
                                                                              activation, functions, f_datas, u_id));

                            blocked.at(static_cast<size_t>(current_pos.x / DecorationTile::SIZE_X_)).
                                    at(static_cast<size_t>(current_pos.y / DecorationTile::SIZE_Y_)) =
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", current_id), "endurance");
                            break;
                        case MapReading::Decorations:
                            decorations.emplace_back(std::make_shared<Decoration>(current_pos, current_id, u_id));
                            break;
                        case MapReading::Weapons:
                            weapons.emplace_back(std::make_shared<PlacedWeapon>(current_pos, direction, current_id,
                                    usage, usage_data, u_id));
                            break;
                        default:
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

    return {{obstacles_tiles, decorations_tiles, characters, specials, obstacles, decorations, weapons},
            {map_size, blocked}};
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

    auto add_functional_obj_to_file = [&file, &map_constraints](const std::string& category, auto& objects) {
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

    add_obj_to_file("decorations", map.getList<Decoration>());
    add_functional_obj_to_file("obstacles", map.getList<Obstacle>());
    add_functional_obj_to_file("characters", map.getList<NPC>());
    add_functional_obj_to_file("specials", map.getList<Special>());
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
