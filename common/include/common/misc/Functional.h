//
// Created by jul3x on 21.04.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H
#define RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H

#include <functional>
#include <string>

#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>
#include <R3E/j3x/J3X.h>

using namespace r3e;

class Character;

class Functional : public Identifiable, public Unique {
public:
    enum class Activation {
        None = 0,
        OnKill = 1,
        OnUse = 2,
        OnEnter = 3,
        OnCollect = 4
    };

    Functional(std::string  activation, const j3x::List& functions,
               const j3x::List& datas, const std::string& id, int u_id = -1);

    [[nodiscard]] const std::string& getActivationStr() const;
    [[nodiscard]] const std::string& getFunctionsStr() const;
    [[nodiscard]] const std::string& getDatasStr() const;
    [[nodiscard]] Activation getActivation() const;
    [[nodiscard]] const j3x::List& getFunctions() const;
    [[nodiscard]] const j3x::List& getDatas() const;
    [[nodiscard]] const std::string& getTextToUse() const;

    void setFunctions(const j3x::List& func);
    void setDatas(const j3x::List& data);
    void setActivation(Activation activation);
    void setActivationStr(const std::string& str);
    void setFunctionsStr(const std::string& str);
    void setDatasStr(const std::string& str);

    [[nodiscard]] virtual bool isUsableByNPC() const;
    [[nodiscard]] virtual bool isActive() const;
    [[nodiscard]] virtual bool isDestroyed() const;
    virtual void activate();
    virtual void deactivate();
    virtual void destroy();

    void bindFunction(const std::function<void(Functional*, const j3x::Obj&, Character*)>& func, const std::string& text,
                      bool is_usable_by_npc);
    virtual void use(Character* user);

private:
    bool is_active_;
    bool is_destroyed_;

    Activation activation_;
    j3x::List functions_, datas_;

    std::vector<std::function<void(Functional*, const j3x::Obj&, Character*)>> funcs_;

    const std::string* text_to_use_;

    bool is_usable_by_npc_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H
