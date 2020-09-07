//
// Created by jul3x on 21.04.20.
//

#include <string>
#include <utility>

#include <common/ResourceManager.h>
#include <common/Functional.h>
#include <common/Character.h>


Functional::Functional(std::string activation, const j3x::List& functions,
                       const j3x::List& datas, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        activation_(std::move(activation)),
        is_active_(true),
        is_destroyed_(false),
        text_to_use_(nullptr),
        is_usable_by_npc_(true)
{
    functions_.reserve(functions.size());
    for (const auto& val : functions) {
        functions_.emplace_back(j3x::getElem<std::string>(val));
    }
    datas_.reserve(datas.size());
    for (const auto& val : datas) {
        datas_.emplace_back(j3x::getElem<std::string>(val));
    }
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
        result += function + j3x::DELIMITER_;
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
        result += data + j3x::DELIMITER_;
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

void Functional::setFunctions(const std::vector<std::string>& func)
{
    functions_ = func;
}

void Functional::setDatas(const std::vector<std::string>& data)
{
    datas_ = data;
}

void Functional::setFunctionsStr(const std::string& str)
{
    j3x::tokenize(str, j3x::DELIMITER_, functions_);
}

void Functional::setDatasStr(const std::string& str)
{
    j3x::tokenize(str, j3x::DELIMITER_, datas_);
}

bool Functional::isActive() const
{
    return is_active_;
}

bool Functional::isDestroyed() const
{
    return is_destroyed_;
}

void Functional::deactivate()
{
    is_active_ = false;
}

void Functional::activate()
{
    is_active_ = true;
}

void Functional::bindFunction(std::function<void(Functional*, const std::string&, Character*)> func, const std::string& text, bool is_usable_by_npc)
{
    funcs_.emplace_back(func);

    if (!(text.empty() && text_to_use_ != nullptr))
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

void Functional::destroy()
{
    is_destroyed_ = true;
}
