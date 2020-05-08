//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>

#include <Editor.h>


using namespace editor;

Editor::Editor() : grid_(CFG.get<int>("window_width_px"), CFG.get<int>("window_height_px"))
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Editor::initialize()
{
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    map_ = std::make_unique<Map>();

    ui_->registerCamera(camera_.get());
    camera_->setViewNormalSize({static_cast<float>(CFG.get<int>("window_width_px")), static_cast<float>(CFG.get<int>("window_height_px"))});

    engine_->initializeGraphics(
            sf::Vector2i{CFG.get<int>("window_width_px"), CFG.get<int>("window_height_px")}, "RAG3 Editor",
            CFG.get<int>("full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CFG.get<int>("background_color")));

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());
}

void Editor::update(float time_elapsed)
{
    camera_->update(time_elapsed);

    auto update_animation = [&time_elapsed](auto& list) {
        for (auto& obj : list)
        {
            obj->updateAnimation(time_elapsed);
        }
    };

    update_animation(map_->getList<DecorationTile>());
    update_animation(map_->getList<Decoration>());
    update_animation(map_->getList<ObstacleTile>());
    update_animation(map_->getList<Obstacle>());
    update_animation(map_->getList<NPC>());
    update_animation(map_->getList<Special>());
    update_animation(map_->getList<PlacedWeapon>());
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

    draw(map_->getList<DecorationTile>());
    draw(map_->getList<Decoration>());
    draw(map_->getList<ObstacleTile>());
    draw(map_->getList<Obstacle>());
    draw(map_->getList<NPC>());
    draw(map_->getList<Special>());
    draw(map_->getList<PlacedWeapon>());

    graphics.drawAlreadySorted();
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
            ui_->openSpecialObjectWindow("characters", ret_npc);
        return read_uid ? ret_npc->getUniqueId() : 0;
    }

    auto ret_obs = map_->getObjectByPos<Obstacle>(pos, ui_->getZIndex());

    if (ret_obs != nullptr)
    {
        if (!read_uid)
            ui_->openSpecialObjectWindow("obstacles", ret_obs);
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

void Editor::placeItem(const sf::Vector2f& pos)
{
    auto max_z_index = ui_->getZIndex();
    if (current_item_.first == "decorations_tiles")
        map_->spawn<DecorationTile>(pos, current_item_.second, true, max_z_index);
    else if (current_item_.first == "obstacles_tiles")
        map_->spawn<ObstacleTile>(pos, current_item_.second, true, max_z_index);
    else if (current_item_.first == "characters")
        map_->spawn<NPC>(pos, current_item_.second, false, max_z_index);
    else if (current_item_.first == "specials")
        map_->spawn<Special>(pos, current_item_.second, false, max_z_index);
    else if (current_item_.first == "decorations")
        map_->spawn<Decoration>(pos, current_item_.second, false, max_z_index);
    else if (current_item_.first == "obstacles")
        map_->spawn<Obstacle>(pos, current_item_.second, false, max_z_index);
    else if (current_item_.first == "weapons")
        map_->spawn<PlacedWeapon>(pos, current_item_.second, false, max_z_index);
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
    auto clean = [](auto& list) {
        for (auto& obj : list)
        {
            obj->setColor(255, 255, 255, 255);
        }
    };

    auto mark = [this](auto ptr, bool add_to_marked_item = true) {
        static auto mark_color = sf::Color(255, 0, 60, 180);
        if (ptr != nullptr)
        {
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
