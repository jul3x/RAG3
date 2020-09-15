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
        is_usable_by_npc_(true),
        functions_(functions),
        datas_(datas)
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

    j3x::serialize(functions_, result);

    return result;
}

const std::string& Functional::getDatasStr() const
{
    static std::string result;
    result.clear();

    j3x::serialize(datas_, result);

    return result;
}

const std::string& Functional::getTextToUse() const
{
    return *text_to_use_;
}

const j3x::List& Functional::getFunctions() const
{
    return functions_;
}

const j3x::List& Functional::getDatas() const
{
    return datas_;
}

void Functional::setActivation(const std::string& str)
{
    activation_ = str;
}

void Functional::setFunctions(const j3x::List& func)
{
    functions_ = func;
}

void Functional::setDatas(const j3x::List& data)
{
    datas_ = data;
}

void Functional::setFunctionsStr(const std::string& str)
{
    functions_ = j3x::parseObj<j3x::List>("list", str);
}

void Functional::setDatasStr(const std::string& str)
{
    datas_ = j3x::parseObj<j3x::List>("list", str);
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

void Functional::bindFunction(const std::function<void(Functional*, const j3x::Obj&, Character*)>& func,
                              const std::string& text, bool is_usable_by_npc)
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
