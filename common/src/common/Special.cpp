//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id, int u_id) :
        Special(position, id,
                utils::j3x::getString(RM.getObjectParams("specials", id), "default_activation"),
                utils::j3x::getListString(RM.getObjectParams("specials", id), "default_functions"),
                utils::j3x::getListString(RM.getObjectParams("specials", id), "default_datas"), u_id)
{

}


Special::Special(const sf::Vector2f& position, const std::string& id,
                 const std::string& activation, const std::vector<std::string>& functions,
                 const std::vector<std::string>& datas, int u_id) :
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
        functions_(functions),
        datas_(datas),
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

const std::string& Special::getFunctionsStr() const
{
    static std::string result;
    result.clear();

    for (auto& function : functions_)
    {
        result += function + ',';
    }

    if (result.length() >= 1)
        result.pop_back();

    return result;
}

const std::string& Special::getDatasStr() const
{
    static std::string result;
    result.clear();

    for (auto& data : datas_)
    {
        result += data + ',';
    }

    if (result.length() >= 1)
        result.pop_back();

    return result;
}

const std::string& Special::getTextToUse() const
{
    return *text_to_use_;
}

const std::vector<std::string>& Special::getFunctions() const
{
    return functions_;
}

const std::vector<std::string>& Special::getDatas() const
{
    return datas_;
}

void Special::setActivation(const std::string& str)
{
    activation_ = str;
}

void Special::setFunctionsStr(const std::string& str)
{
    utils::j3x::tokenize(str, ',', functions_);
}

void Special::setDatasStr(const std::string& str)
{
    utils::j3x::tokenize(str, ',', datas_);
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
    funcs_.emplace_back(func);
    text_to_use_ = &text;
}

void Special::use()
{
    for (size_t i = 0; i < funcs_.size(); ++i)
        funcs_.at(i)(this, datas_.at(i));
}