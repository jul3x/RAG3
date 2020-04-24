//
// Created by jul3x on 21.04.20.
//

#include <string>

#include <common/ResourceManager.h>
#include <common/Functional.h>
#include <common/Character.h>


Functional::Functional(const std::string& activation, const std::vector<std::string>& functions,
                       const std::vector<std::string>& datas, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        activation_(activation),
        functions_(functions),
        datas_(datas),
        is_active_(true),
        text_to_use_(nullptr),
        is_usable_by_npc_(true)
{

}

const std::string& Functional::getActivation() const
{
    return activation_;
}

const std::string& Functional::getFunctionsStr() const
{
    static std::string result;
    result.clear();

    for (auto& function : functions_)
    {
        result += function + utils::j3x::Delimiter;
    }

    if (result.length() >= 1)
        result.pop_back();

    return result;
}

const std::string& Functional::getDatasStr() const
{
    static std::string result;
    result.clear();

    for (auto& data : datas_)
    {
        result += data + utils::j3x::Delimiter;
    }

    if (result.length() >= 1)
        result.pop_back();

    return result;
}

const std::string& Functional::getTextToUse() const
{
    return *text_to_use_;
}

const std::vector<std::string>& Functional::getFunctions() const
{
    return functions_;
}

const std::vector<std::string>& Functional::getDatas() const
{
    return datas_;
}

void Functional::setActivation(const std::string& str)
{
    activation_ = str;
}

void Functional::setFunctionsStr(const std::string& str)
{
    // TODO Delimiter
    utils::j3x::tokenize(str, utils::j3x::Delimiter, functions_);
}

void Functional::setDatasStr(const std::string& str)
{
    utils::j3x::tokenize(str, utils::j3x::Delimiter, datas_);
}

bool Functional::isActive() const
{
    return is_active_;
}

void Functional::deactivate()
{
    is_active_ = false;
}

void Functional::bindFunction(std::function<void(Functional*, const std::string&, Character*)> func, const std::string& text, bool is_usable_by_npc)
{
    funcs_.emplace_back(func);
    text_to_use_ = &text;

    if (!is_usable_by_npc)
        is_usable_by_npc_ = false;
}

void Functional::use(Character* user)
{
    for (size_t i = 0; i < funcs_.size(); ++i)
        funcs_.at(i)(this, datas_.at(i), user);
}

bool Functional::isUsableByNPC() const
{
    return is_usable_by_npc_;
}