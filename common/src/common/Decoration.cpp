//
// Created by jul3x on 20.03.20.
//

#include <common/Decoration.h>
#include <common/ResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        AbstractDrawableObject(position,
                               RMGET<sf::Vector2f>("decorations", id, "size"),
                               &RM.getTexture("decorations/" + id),
                               RMGET<int>("decorations", id, "z_index"),
                               RMGET<int>("decorations", id, "frames_number"),
                               RMGET<float>("decorations", id, "frame_duration")),
        is_active_(true)
{
    this->changeOrigin(RMGET<sf::Vector2f>("decorations", id, "size") / 2.0f +
                       RMGET<sf::Vector2f>("decorations", id, "map_offset"));

    if (RMGET<bool>("decorations", id, "light_point"))
    {
        float light_size = CONF<float>("graphics/decorations_light_point_size") * CONF<float>("graphics/global_zoom");
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{light_size, light_size},
                                                        &RM.getTexture("lightpoint"));
    }
}

bool Decoration::isActive() const
{
    return is_active_;
}

void Decoration::deactivate()
{
    is_active_ = false;
}
