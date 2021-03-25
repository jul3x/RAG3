//
// Created by jul3x on 16.05.2020.
//

#ifndef RAG3_GAME_INCLUDE_MISC_DEBUGMAPBLOCKAGE_H
#define RAG3_GAME_INCLUDE_MISC_DEBUGMAPBLOCKAGE_H

#include <vector>

#include <common/ResourceManager.h>


using namespace r3e;

class DebugMapBlockage {
public:
    explicit DebugMapBlockage(ai::MapBlockage* map_blockage)
    {
        map_blockage_ = map_blockage;

        int i = 0, j = 0;
        for (const auto& x : map_blockage->blockage_)
        {
            j = 0;
            for (const auto& y : x)
            {
                grid_.emplace_back(sf::Vector2f{i * DecorationTile::SIZE_X_, j * DecorationTile::SIZE_Y_},
                                   sf::Vector2f{DecorationTile::SIZE_X_, DecorationTile::SIZE_Y_},
                                   &RM.getTexture("health_bar"));

                if (y != 0.0f)
                    grid_.back().setColor(255, 0, 0, 255);
                else
                    grid_.back().setColor(0, 255, 0, 255);

                ++j;
            }
            ++i;
        }
    }

    void draw(graphics::Graphics& graphics)
    {
        int i = 0, j = 0;
        for (const auto& x : map_blockage_->blockage_)
        {
            j = 0;
            for (const auto& y : x)
            {
                if (y != 0.0f)
                    grid_.at(map_blockage_->blockage_.front().size() * i + j).setColor(255, 0, 0, 255);
                else
                    grid_.at(map_blockage_->blockage_.front().size() * i + j).setColor(0, 255, 0, 255);

                ++j;
            }
            ++i;
        }

        for (auto& el : grid_)
        {
            graphics.draw(el);
        }

        static sf::Text text("", RM.getFont(), 16);
        i = 0;
        for (const auto& x : map_blockage_->blockage_)
        {
            j = 0;
            for (const auto& y : x)
            {
                text.setString(std::to_string(static_cast<int>(std::min(y, 100.0f))));
                text.setPosition(DecorationTile::SIZE_X_ * i, DecorationTile::SIZE_Y_ * j);
                graphics.draw(text);
                ++j;
            }
            ++i;
        }
    }

private:
    ai::MapBlockage* map_blockage_;
    std::vector<AbstractDrawableObject> grid_;

};

#endif //RAG3_GAME_INCLUDE_MISC_DEBUGMAPBLOCKAGE_H
