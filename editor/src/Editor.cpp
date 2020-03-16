//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>

#include <editor/ResourceManager.h>

#include <Editor.h>


using namespace editor;

Editor::Editor() : grid_(CFG.getInt("window_width_px"), CFG.getInt("window_height_px"))
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
    camera_->setViewNormalSize({static_cast<float>(CFG.getInt("window_width_px")), static_cast<float>(CFG.getInt("window_height_px"))});

    engine_->initializeGraphics(
            sf::Vector2i{CFG.getInt("window_width_px"), CFG.getInt("window_height_px")}, "Rag3 Editor",
            CFG.getInt("full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CFG.getInt("background_color")));

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());
}

void Editor::update(float time_elapsed)
{
    camera_->update(time_elapsed);
    map_->getTileConstraints();
}

void Editor::draw(graphics::Graphics& graphics)
{
    grid_.draw(graphics);

    for (auto& decoration : map_->getDecorationsTiles())
        graphics.draw(decoration);

    for (auto& obstacle : map_->getObstaclesTiles())
        graphics.drawSorted(obstacle);

    for (auto& character : map_->getCharacters())
        graphics.drawSorted(character);

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

void Editor::placeItem(const sf::Vector2f& pos)
{
    if (current_item_.first == "decorations_tiles")
        map_->spawnDecorationTile(pos, current_item_.second);
    else if (current_item_.first == "obstacles_tiles")
        map_->spawnObstacleTile(pos, current_item_.second);
}

void Editor::removeItem(const sf::Vector2f& pos)
{
    if (current_item_.first == "decorations_tiles" || current_item_.first == "obstacles_tiles")
        map_->removeTile(pos);
}
