//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        AbstractDrawableObject(position,
                               {utils::getFloat(RM.getObjectParams("specials", id), "size_x"),
                                utils::getFloat(RM.getObjectParams("specials", id), "size_y")},
                               &RM.getTexture("specials/" + id))
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("specials", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("specials", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("specials", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("specials", id), "map_offset_y")));
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