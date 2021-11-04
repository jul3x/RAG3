//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>

#include <Editor.h>


using namespace editor;

Editor::Editor() : grid_(CONF<int>("window_width_px"), CONF<int>("window_height_px")),
                   is_lighting_on_(true), randomizing_value_(0), marked_item_(nullptr)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
    setGridMarked(false);
}

void Editor::initialize()
{
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    map_ = std::make_unique<Map>();

    ui_->registerCamera(camera_.get());
    camera_->setViewNormalSize(
            {static_cast<float>(CONF<int>("window_width_px")), static_cast<float>(CONF<int>("window_height_px"))});

    engine_->initializeGraphics(
            sf::Vector2i{CONF<int>("window_width_px"), CONF<int>("window_height_px")}, "RAG3 Editor",
            CONF<bool>("full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CONF<int>("background_color")));

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    lighting_ = std::make_unique<graphics::Lighting>(sf::Vector2f{static_cast<float>(CONF<int>("window_width_px")),
                                                                  static_cast<float>(CONF<int>("window_height_px"))},
                                                     sf::Color(CONF<int>("graphics/lighting_color")));

}

void Editor::update(float time_elapsed)
{
    camera_->update(time_elapsed);

    auto update_obj = [&time_elapsed](auto& list) {
        for (auto& obj : list)
        {
            obj->updateAnimation(time_elapsed);
        }
    };

    auto update_light = [](auto& list) {
        for (auto& obj : list)
        {
            auto light = obj->getLightPoint();

            if (light != nullptr)
                light->setPosition(obj->getPosition());
        }
    };

    update_obj(map_->getList<DecorationTile>());
    update_obj(map_->getList<Decoration>());
    update_obj(map_->getList<ObstacleTile>());
    update_obj(map_->getList<Obstacle>());
    update_obj(map_->getList<NPC>());
    update_obj(map_->getList<Special>());
    update_obj(map_->getList<PlacedWeapon>());
    update_light(map_->getList<Decoration>());
    update_light(map_->getList<Obstacle>());
    update_light(map_->getList<NPC>());
    update_light(map_->getList<Special>());
}

void Editor::draw(graphics::Graphics& graphics)
{
    grid_.draw(graphics);
    auto max_z_index = ui_->getZIndex();

    auto draw = [&graphics, &max_z_index](auto& list) {
        for (auto& obj : list)
            if (obj->getZIndex() <= max_z_index)
                graphics.drawSorted(*obj);
    };

    auto draw_light = [&graphics, &max_z_index, this](auto& list) {
        for (auto& obj : list)
            if (obj->getZIndex() <= max_z_index)
                if (obj->getLightPoint() != nullptr)
                    this->lighting_->add(*obj->getLightPoint());
    };

    draw(map_->getList<DecorationTile>());
    draw(map_->getList<Decoration>());
    draw(map_->getList<ObstacleTile>());
    draw(map_->getList<Obstacle>());
    draw(map_->getList<NPC>());
    draw(map_->getList<Special>());
    draw(map_->getList<PlacedWeapon>());

    graphics.drawAlreadySorted();

    if (is_lighting_on_)
    {
        lighting_->clear();
        draw_light(map_->getList<NPC>());
        draw_light(map_->getList<Obstacle>());
        draw_light(map_->getList<Decoration>());
        draw_light(map_->getList<Special>());
        graphics.setStaticView();
        graphics.draw(*lighting_);
    }
}

void Editor::start()
{
    engine_->start();
}

void Editor::setCurrentItem(const std::string& category, const std::string& id)
{
    current_item_.first = category;
    current_item_.second = id;
}

const std::pair<std::string, std::string>& Editor::getCurrentItem() const
{
    return current_item_;
}

const std::string& Editor::getCurrentMapName() const
{
    return current_map_name_;
}

sf::Vector2f Editor::getMapCoordinates(const sf::Vector2f& pos) const
{
    auto map_constraints = map_->getTileConstraints();

    return {pos.x - map_constraints.second.x, pos.y - map_constraints.second.y};
}

int Editor::readItemInfo(const sf::Vector2f& pos, bool read_uid)
{
    auto ret_special = map_->getObjectByPos<Special>(pos, ui_->getZIndex());

    if (ret_special != nullptr)
    {
        if (!read_uid)
            ui_->openSpecialObjectWindow("specials", ret_special);
        return read_uid ? ret_special->getUniqueId() : 0;
    }

    auto ret_npc = map_->getObjectByPos<NPC>(pos, ui_->getZIndex());

    if (ret_npc != nullptr)
    {
        if (!read_uid)
            ui_->openCharacterObjectWindow("characters", ret_npc);
        return read_uid ? ret_npc->getUniqueId() : 0;
    }

    auto ret_obs = map_->getObjectByPos<Obstacle>(pos, ui_->getZIndex());

    if (ret_obs != nullptr)
    {
        if (!read_uid)
            ui_->openObstacleWindow("obstacles", ret_obs);
        return read_uid ? ret_obs->getUniqueId() : 0;
    }

    auto ret_weapon = map_->getObjectByPos<PlacedWeapon>(pos, ui_->getZIndex());

    if (ret_weapon != nullptr)
    {
        if (!read_uid)
            ui_->openWeaponWindow("weapons", ret_weapon);
        return read_uid ? ret_weapon->getUniqueId() : 0;
    }

    auto ret = map_->getObjectInfo(pos, ui_->getZIndex());

    if (!(std::get<0>(ret).empty() && std::get<1>(ret).empty()))
    {
        if (!read_uid)
            ui_->openUniqueObjectWindow(std::get<0>(ret), std::get<1>(ret), std::get<2>(ret));
        return read_uid ? std::get<2>(ret) : 0;
    }

    return -1;
}

void Editor::placeItem(const sf::Vector2f& pos, float direction)
{
    auto max_z_index = ui_->getZIndex();

    try
    {
        if (current_item_.first == "decorations_tiles")
        {
            auto number = std::stoi(current_item_.second) + utils::num::getRandom(0, randomizing_value_);
            auto ptr = map_->spawn<DecorationTile>(pos, direction, std::to_string(number), true, max_z_index);
        }
        else if (current_item_.first == "obstacles_tiles")
        {
            auto number = std::stoi(current_item_.second) + utils::num::getRandom(0, randomizing_value_);
            auto ptr = map_->spawn<ObstacleTile>(pos, direction, std::to_string(number), true, max_z_index);
        }
        else if (current_item_.first == "characters")
        {
            auto ptr = map_->spawn<NPC>(pos, direction, current_item_.second, false, max_z_index);
            if (ptr->getLightPoint() != nullptr)
                ptr->getLightPoint()->registerGraphics(engine_->getGraphics());
        }
        else if (current_item_.first == "specials")
        {
            auto ptr = map_->spawn<Special>(pos, direction, current_item_.second, false, max_z_index);
            if (ptr->getLightPoint() != nullptr)
                ptr->getLightPoint()->registerGraphics(engine_->getGraphics());
        }

        else if (current_item_.first == "decorations")
        {
            auto ptr = map_->spawn<Decoration>(pos, direction, current_item_.second, false, max_z_index);
            if (ptr->getLightPoint() != nullptr)
                ptr->getLightPoint()->registerGraphics(engine_->getGraphics());
        }

        else if (current_item_.first == "obstacles")
        {
            auto ptr = map_->spawn<Obstacle>(pos, direction, current_item_.second, false, max_z_index);
            if (ptr->getLightPoint() != nullptr)
                ptr->getLightPoint()->registerGraphics(engine_->getGraphics());
        }
        else if (current_item_.first == "weapons")
            auto ptr = map_->spawn<PlacedWeapon>(pos, direction, current_item_.second, false, max_z_index);
    }
    catch (const std::logic_error& err)
    {
        spawnError("Object to place not found!");
    }
}

void Editor::removeItem(const sf::Vector2f& pos)
{
    auto max_z_index = ui_->getZIndex();
    if (current_item_.first == "decorations_tiles" || current_item_.first == "obstacles_tiles")
        map_->removeTile(pos, max_z_index);
    else if (current_item_.first == "characters" || current_item_.first == "specials" ||
             current_item_.first == "decorations" || current_item_.first == "obstacles" ||
             current_item_.first == "weapons")
        map_->removeObject(pos, max_z_index);
}

void Editor::clearMap()
{
    if (map_->clearMap())
    {
        camera_->setPointingTo({});
        ui_->spawnInfo("Map cleared!");
        current_map_name_ = "";
    }
    else
    {
        ui_->spawnError("Map clearing failed!");
    }
}

void Editor::loadMap(const std::string& name)
{
    if (map_->loadMap(name))
    {
        camera_->setPointingTo({});
        ui_->spawnInfo("Map " + name + " succesfully loaded!");
        current_map_name_ = name;

        auto init_light = [this](auto& list) {
            for (auto& obj : list)
                if (obj->getLightPoint() != nullptr)
                    obj->getLightPoint()->registerGraphics(engine_->getGraphics());
        };

        init_light(map_->getList<NPC>());
        init_light(map_->getList<Obstacle>());
        init_light(map_->getList<Decoration>());
        init_light(map_->getList<Special>());
    }
    else
    {
        ui_->spawnError("Map loading failed!");
    }
}

void Editor::saveMap(const std::string& name)
{
    if (RM.saveMap(name, *map_))
    {
        ui_->spawnInfo("Map " + name + " succesfully saved!");
        current_map_name_ = name;
    }
    else
    {
        ui_->spawnError("Map saving failed!");
    }
}

void Editor::saveConfig(const std::string& category, const std::string& id, const std::string& content)
{
    if (RM.saveConfigFile(category, id, content))
    {
        ui_->spawnInfo("Config file " + category + "/" + id + " succesfully saved!");
    }
    else
    {
        ui_->spawnError("Config file saving failed!");
    }
}

void Editor::markCurrentItem(const sf::Vector2f& pos)
{
    marked_item_ = nullptr;
    auto clean = [this](auto& list) {
        for (auto& obj : list)
        {
            if (!grid_marked_items_.count(obj.get()))
                obj->setColor(255, 255, 255, 255);
        }
    };

    auto mark = [this](auto ptr, bool add_to_marked_item = true) {
        static auto mark_color = sf::Color(255, 0, 60, 180);
        if (ptr != nullptr)
        {
            if (grid_marked_items_.count(ptr))
                return;

            ptr->setColor(mark_color.r, mark_color.g, mark_color.b, mark_color.a);

            if (add_to_marked_item)
                this->marked_item_ = ptr;
        }
    };

    clean(map_->getList<ObstacleTile>());
    clean(map_->getList<Obstacle>());
    clean(map_->getList<Decoration>());
    clean(map_->getList<Special>());
    clean(map_->getList<NPC>());
    clean(map_->getList<PlacedWeapon>());

    mark(map_->getObjectByPos<ObstacleTile>(pos, ui_->getZIndex()), false);
    mark(map_->getObjectByPos<Obstacle>(pos, ui_->getZIndex()));
    mark(map_->getObjectByPos<Decoration>(pos, ui_->getZIndex()));
    mark(map_->getObjectByPos<NPC>(pos, ui_->getZIndex()));
    mark(map_->getObjectByPos<Special>(pos, ui_->getZIndex()));
    mark(map_->getObjectByPos<PlacedWeapon>(pos, ui_->getZIndex()));
}

AbstractDrawableObject* Editor::getMarkedItem()
{
    return marked_item_;
}

void Editor::spawnError(const std::string& err)
{
    ui_->spawnError(err);
}

void Editor::resetLighting(const sf::Vector2f& size)
{
    lighting_ = std::make_unique<graphics::Lighting>(size, sf::Color(CONF<int>("graphics/lighting_color")));
}

void Editor::setLighting(bool on)
{
    is_lighting_on_ = on;
}

void Editor::setRandomizing(int n)
{
    randomizing_value_ = n;
}

void Editor::close()
{
    this->engine_->getGraphics().getWindow().close();
}

Map& Editor::getMap()
{
    return *map_;
}

UserInterface& Editor::getUI()
{
    return *ui_;
}

void Editor::setMarkingStart(const sf::Vector2f& pos)
{
    grid_mark_start_ = pos;
}

void Editor::setMarkingStop(const sf::Vector2f& pos)
{
    grid_mark_stop_ = pos;
}

void Editor::setGridMarked(bool marked)
{
    is_grid_marked_ = marked;

    if (marked)
    {
        auto z_index = ui_->getZIndex();
        auto grid_pos = sf::Vector2f(std::min(grid_mark_start_.x, grid_mark_stop_.x),
                                     std::min(grid_mark_start_.y, grid_mark_stop_.y));
        auto grid_size = sf::Vector2f(std::abs(grid_mark_start_.x - grid_mark_stop_.x),
                                      std::abs(grid_mark_start_.y - grid_mark_stop_.y));

        auto mark_items = [this, &grid_pos, &grid_size, z_index](auto& container) {
            for (const auto& item : container)
            {
                const auto& pos = item->getPosition();
                if (item->getZIndex() <= z_index && utils::geo::isPointInRectangle(pos, grid_pos, grid_size))
                {
                    grid_marked_items_.insert(item.get());
                    item->setColor(0, 255, 60, 180);
                }
            }
        };

        mark_items(map_->getList<DecorationTile>());
        mark_items(map_->getList<ObstacleTile>());
        mark_items(map_->getList<Obstacle>());
        mark_items(map_->getList<Decoration>());
        mark_items(map_->getList<Special>());
        mark_items(map_->getList<NPC>());
        mark_items(map_->getList<PlacedWeapon>());
    }
    else
    {
        for (auto items : grid_marked_items_)
            items->setColor(255, 255, 255, 255);

        grid_marked_items_.clear();
    }
}

void Editor::removeMarkedItems()
{
    utils::eraseIf<std::shared_ptr<DecorationTile>>(map_->getList<DecorationTile>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<ObstacleTile>>(map_->getList<ObstacleTile>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<Obstacle>>(map_->getList<Obstacle>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<Decoration>>(map_->getList<Decoration>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<Special>>(map_->getList<Special>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<NPC>>(map_->getList<NPC>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });
    utils::eraseIf<std::shared_ptr<PlacedWeapon>>(map_->getList<PlacedWeapon>(), [this](auto& item) {
        return grid_marked_items_.count(item.get());
    });

    grid_marked_items_.clear();
}

const sf::Vector2f& Editor::getMarkingStart() const
{
    return grid_mark_start_;
}

const sf::Vector2f& Editor::getMarkingStop() const
{
    return grid_mark_stop_;
}

bool Editor::isGridMarked() const
{
    return is_grid_marked_;
}

void Editor::setGridMarkedPositions(const sf::Vector2f& pos_offset)
{
    if (!isGridMarked())
        return;

    for (auto ptr : grid_marked_items_)
    {
        ptr->setPosition(ptr->getPosition() + pos_offset);
    }
}
