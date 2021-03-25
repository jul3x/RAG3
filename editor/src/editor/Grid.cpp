//
// Created by jul3x on 15.03.20.
//

#include <common/objects/DecorationTile.h>

#include <editor/Grid.h>


using namespace editor;

Grid::Grid(float size_x, float size_y) : size_(size_x, size_y), array_(sf::Lines, 0)
{
}

void Grid::draw(graphics::Graphics& graphics)
{
    auto diff_x = static_cast<size_t>(size_.x / DecorationTile::SIZE_X_);
    auto diff_y = static_cast<size_t>(size_.y / DecorationTile::SIZE_Y_);

    static auto size_x_2 = DecorationTile::SIZE_X_ / 2.0f;
    static auto size_y_2 = DecorationTile::SIZE_Y_ / 2.0f;

    array_.resize(4 * (diff_x + diff_y));

    for (size_t i = 0; i < 2 * diff_x; i += 2)
    {
        auto current_increment = i / 2.0f * DecorationTile::SIZE_X_;
        array_[i].position = sf::Vector2f(size_x_2 + current_increment, size_y_2 - size_.y);
        array_[i + 1].position = sf::Vector2f(size_x_2 + current_increment, size_y_2 + size_.y);
    }

    for (size_t i = 2 * diff_x; i < 4 * diff_x; i += 2)
    {
        auto current_increment = (i - 2 * diff_x) / 2.0f * DecorationTile::SIZE_X_;
        array_[i].position = sf::Vector2f(size_x_2 - current_increment, size_y_2 - size_.y);
        array_[i + 1].position = sf::Vector2f(size_x_2 - current_increment, size_y_2 + size_.y);
    }

    for (size_t i = 4 * diff_x; i < 4 * diff_x + 2 * diff_y; i += 2)
    {
        auto current_increment = (i - 4 * diff_x) / 2.0f * DecorationTile::SIZE_Y_;
        array_[i].position = sf::Vector2f(size_x_2 - size_.x, size_y_2 + current_increment);
        array_[i + 1].position = sf::Vector2f(size_x_2 + size_.x, size_y_2 + current_increment);
    }

    for (size_t i = 4 * diff_x + 2 * diff_y; i < 4 * diff_x + 4 * diff_y; i += 2)
    {
        auto current_increment = (i - 4 * diff_x - 2 * diff_y) / 2.0f * DecorationTile::SIZE_Y_;
        array_[i].position = sf::Vector2f(size_x_2 - size_.x, size_y_2 - current_increment);
        array_[i + 1].position = sf::Vector2f(size_x_2 + size_.x, size_y_2 - current_increment);
    }

    for (size_t i = 0; i < 4 * (diff_x + diff_y); ++i)
    {
        array_[i].color = sf::Color(255, 255, 255, 60);
    }

    graphics.getWindow().draw(array_);
}

