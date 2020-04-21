//
// Created by jul3x on 21.04.20.
//

#include <string>

#include <common/ResourceManager.h>
#include <common/Functional.h>


Functional::Functional(const std::string& activation, const std::vector<std::string>& functions,
                       const std::vector<std::string>& datas, const std::string& id, int u_id) :
        Identifiable(id),
        Unique(u_id),
        activation_(activation),
        functions_(functions),
        datas_(datas),
        is_active_(true),
        text_to_use_(nullptr)
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
        result += function + ';';
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
        result += data + ';';
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
    utils::j3x::tokenize(str, ';', functions_);
}

void Functional::setDatasStr(const std::string& str)
{
    utils::j3x::tokenize(str, ';', datas_);
}

bool Functional::isActive() const
{
    return is_active_;
}

void Functional::deactivate()
{
    is_active_ = false;
}

void Functional::bindFunction(std::function<void(Functional*, const std::string&)> func, const std::string& text)
{
    funcs_.emplace_back(func);
    text_to_use_ = &text;
}

void Functional::use()
{
    for (size_t i = 0; i < funcs_.size(); ++i)
        funcs_.at(i)(this, datas_.at(i));
}