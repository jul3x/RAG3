//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id, int u_id) :
        Special(position, id,
                utils::j3x::getString(RM.getObjectParams("specials", id), "default_activation"),
                utils::j3x::getString(RM.getObjectParams("specials", id), "default_function"),
                utils::j3x::getString(RM.getObjectParams("specials", id), "default_data"), u_id)
{

}


Special::Special(const sf::Vector2f& position, const std::string& id,
                 const std::string& activation, const std::string& function,
                 const std::string& data, int u_id) :
        Identifiable(id),
        Unique(u_id),
        HoveringObject(position, {},
                       {utils::j3x::getFloat(RM.getObjectParams("specials", id), "size_x"),
                        utils::j3x::getFloat(RM.getObjectParams("specials", id), "size_y")},
                       Collision::Circle(utils::j3x::getFloat(RM.getObjectParams("specials", id), "collision_size_x")),
                       &RM.getTexture("specials/" + id),
                       utils::j3x::getInt(RM.getObjectParams("specials", id), "frames_number"),
                       utils::j3x::getFloat(RM.getObjectParams("specials", id), "frame_duration"),
                       0.0f),
        activation_(activation),
        function_(function),
        data_(data),
        is_drawable_(utils::j3x::getInt(RM.getObjectParams("specials", id), "is_drawable")),
        is_active_(true),
        text_to_use_(nullptr)
{
    this->changeOrigin(sf::Vector2f(utils::j3x::getFloat(RM.getObjectParams("specials", id), "size_x"),
                                    utils::j3x::getFloat(RM.getObjectParams("specials", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::getFloat(RM.getObjectParams("specials", id), "map_offset_x"),
                                    utils::j3x::getFloat(RM.getObjectParams("specials", id), "map_offset_y")));
}

bool Special::isDrawable() const
{
    return is_drawable_;
}

const std::string& Special::getActivation() const
{
    return activation_;
}

const std::string& Special::getFunction() const
{
    return function_;
}

const std::string& Special::getData() const
{
    return data_;
}

const std::string& Special::getTextToUse() const
{
    return *text_to_use_;
}

void Special::setActivation(const std::string& str)
{
    activation_ = str;
}

void Special::setFunction(const std::string& str)
{
    function_ = str;
}

void Special::setData(const std::string& str)
{
    data_ = str;
}

bool Special::isActive() const
{
    return is_active_;
}

void Special::deactivate()
{
    is_active_ = false;
}

void Special::bindFunction(std::function<void(Special*, const std::string&)> func, const std::string& text)
{
    func_ = func;
    text_to_use_ = &text;
}

void Special::use()
{
    func_(this, data_);
}