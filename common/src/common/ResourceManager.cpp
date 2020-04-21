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
        Decorations
    };
    auto map_reading = MapReading::None;

    std::ifstream file(CFG.getString("paths/maps_dir") + "/" + key + ".j3x");
    std::list<std::shared_ptr<ObstacleTile>> obstacles_tiles;
    std::list<std::shared_ptr<DecorationTile>> decorations_tiles;
    std::list<std::shared_ptr<NPC>> characters;
    std::list<std::shared_ptr<Special>> specials;
    std::list<std::shared_ptr<Obstacle>> obstacles;
    std::list<std::shared_ptr<Decoration>> decorations;
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
        int u_id = -1;

        std::string activation = "";
        std::string function = "";
        std::string f_data = "";

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
                                    utils::j3x::getFloat(RM.getObjectParams("obstacles_tiles", std::to_string(type)), "endurance");
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
                    case MapReading::Characters:
                    case MapReading::Obstacles:
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
                    case MapReading::Specials:
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
                            function = word;
                        }
                        else
                        {
                            std::replace(word.begin(), word.end(), '_', ' ');
                            f_data = word;
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
                            characters.emplace_back(std::make_shared<NPC>(current_pos, current_id, u_id));
                            break;
                        case MapReading::Specials:
                            specials.emplace_back(std::make_shared<Special>(current_pos, current_id,
                                                                            activation, function, f_data, u_id));
                            break;
                        case MapReading::Obstacles:
                            obstacles.emplace_back(std::make_shared<Obstacle>(current_pos, current_id, u_id));
                            blocked.at(static_cast<size_t>(current_pos.x / DecorationTile::SIZE_X_)).
                                    at(static_cast<size_t>(current_pos.y / DecorationTile::SIZE_Y_)) =
                                    utils::j3x::getFloat(RM.getObjectParams("obstacles", current_id), "endurance");
                            break;
                        case MapReading::Decorations:
                            decorations.emplace_back(std::make_shared<Decoration>(current_pos, current_id, u_id));
                            break;
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

    return {{obstacles_tiles, decorations_tiles, characters, specials, obstacles, decorations},
            {map_size, blocked}};
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
        matrix.at(static_cast<size_t>((obstacle->getPosition().y - map_constraints.second.y) / DecorationTile::SIZE_Y_)).
               at(static_cast<size_t>((obstacle->getPosition().x - map_constraints.second.x) / DecorationTile::SIZE_X_)) = std::stoi(obstacle->getId());
    }

    for (const auto& decoration : map.getDecorationsTiles())
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

    auto addObjToFile = [&file, &map_constraints](const std::string& category, auto& objects) {
        file << std::endl << category << ": " << std::endl;
        for (const auto& obj : objects)
        {
            file << obj->getId() << " " <<
                 obj->getPosition().x - map_constraints.second.x << " " <<
                 obj->getPosition().y - map_constraints.second.y << " " <<
                 obj->getUniqueId() << std::endl;
        }
    };

    addObjToFile("characters", map.getNPCs());
    addObjToFile("obstacles", map.getObstacles());
    addObjToFile("decorations", map.getDecorations());

    file << std::endl << "specials: " << std::endl;
    for (const auto& obj : map.getSpecials())
    {
        auto new_data = obj->getData();
        std::replace(new_data.begin(), new_data.end(), ' ', '_');

        file << obj->getId() << " " <<
             obj->getPosition().x - map_constraints.second.x << " " <<
             obj->getPosition().y - map_constraints.second.y << " " <<
             obj->getUniqueId() << " " <<
             obj->getActivation() << " " <<
             obj->getFunction() << " " <<
             new_data << std::endl;
    }

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

ResourceManager::ResourceManager() : AbstractResourceManager(CFG.getString("paths/j3x_dir"), CFG.getString("paths/textures_dir"), CFG.getString("paths/fonts_dir"),
                                                             CFG.getString("paths/sounds_dir"), CFG.getString("paths/music_dir"))
{

}
