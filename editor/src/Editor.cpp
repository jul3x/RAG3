//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>
#include <editor/ResourceManager.h>

#include <Editor.h>

using namespace editor;

Editor::Editor()
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Editor::initialize()
{
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    map_ = std::make_unique<Map>();

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
}

void Editor::draw(graphics::Graphics& graphics)
{
    for (auto& decoration : map_->getDecorations())
        graphics.draw(decoration);

    for (auto& obstacle : map_->getObstacles())
        graphics.drawSorted(obstacle);

    for (auto& character : map_->getCharacters())
        graphics.drawSorted(character);

    graphics.drawAlreadySorted();
}

void Editor::start()
{
    engine_->start();
}

