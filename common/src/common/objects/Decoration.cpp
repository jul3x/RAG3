//
// Created by jul3x on 20.03.20.
//

#include <common/objects/Decoration.h>
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

    this->makeLightPoint(this->getPosition(),
                         RMGET<float>("decorations", id, "light_point_radius", true) *
                         CONF<float>("graphics/global_zoom"),
                         &RM.getTexture("lightpoint"), RMGET<std::string>("decorations", id, "light_point"),
                         RMGET<float>("decorations", id, "light_point_data", true));
}

bool Decoration::isActive() const
{
    return is_active_;
}

void Decoration::deactivate()
{
    is_active_ = false;
}
