//
// Created by jul3x on 15.03.020.
//

#include <editor/Grid.h>
#include <editor/Decoration.h>


using namespace editor;

Grid::Grid(float size_x, float size_y) : size_(size_x, size_y), array_(sf::Lines, 0)
{

}

void Grid::draw(graphics::Graphics& graphics)
{
    array_.resize(2 * size_.x / Decoration::SIZE_X_ + 2 * size_.y / Decoration::SIZE_Y_);

    for (int i = 0; i < 2 * size_.y / Decoration::SIZE_Y_; i += 2)
    {
        array_[i].position = sf::Vector2f(i * Decoration::SIZE_Y_, 0.0f);
        array_[i+1].position = sf::Vector2f(i * Decoration::SIZE_Y_, size_.y);

        array_[i].color = sf::Color::White;
        array_[i+1].color = sf::Color::White;
    }

    for (int i = 0; i < 2 * size_.x / Decoration::SIZE_X_; i += 2)
    {
        auto offset = 2 * size_.y / Decoration::SIZE_Y_;
        array_[i + offset - 1].position = sf::Vector2f(0.0f, i * Decoration::SIZE_X_);
        array_[i+1 + offset - 1].position = sf::Vector2f(size_.x, i * Decoration::SIZE_X_);

        array_[i + offset - 1].color = sf::Color::White;
        array_[i+1 + offset - 1].color = sf::Color::White;
    }

    graphics.getWindow().draw(array_);
}

