//
// Created by jul3x on 20.03.20.
//

#include <common/Decoration.h>
#include <common/ResourceManager.h>


Decoration::Decoration(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        AbstractDrawableObject(position,
                               j3x::get<sf::Vector2f>(RM.getObjectParams("decorations", id), "size"),
                               &RM.getTexture("decorations/" + id),
                               j3x::get<int>(RM.getObjectParams("decorations", id), "z_index"),
                               j3x::get<int>(RM.getObjectParams("decorations", id), "frames_number"),
                               j3x::get<float>(RM.getObjectParams("decorations", id), "frame_duration")),
        is_active_(true)
{
    this->changeOrigin(j3x::get<sf::Vector2f>(RM.getObjectParams("decorations", id), "size") / 2.0f +
                       j3x::get<sf::Vector2f>(RM.getObjectParams("decorations", id), "map_offset"));

    if (j3x::get<bool>(RM.getObjectParams("decorations", id), "light_point"))
    {
        float light_size = CFG.get<float>("graphics/decorations_light_point_size") * CFG.get<float>("graphics/global_zoom");
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
