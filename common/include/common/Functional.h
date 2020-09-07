//
// Created by jul3x on 21.04.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H
#define RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H

#include <functional>
#include <string>

#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>


using namespace r3e;

class Character;

class Functional : public Identifiable, public Unique {
public:
    Functional(std::string  activation, const j3x::List& functions,
               const j3x::List& datas, const std::string& id, int u_id = -1);

    [[nodiscard]] const std::string& getActivation() const;
    [[nodiscard]] const std::string& getFunctionsStr() const;
    [[nodiscard]] const std::string& getDatasStr() const;
    [[nodiscard]] const std::vector<std::string>& getFunctions() const;
    [[nodiscard]] const std::vector<std::string>& getDatas() const;
    [[nodiscard]] const std::string& getTextToUse() const;

    void setFunctions(const std::vector<std::string>& func);
    void setDatas(const std::vector<std::string>& data);
    void setActivation(const std::string& str);
    void setFunctionsStr(const std::string& str);
    void setDatasStr(const std::string& str);

    [[nodiscard]] virtual bool isUsableByNPC() const;
    [[nodiscard]] virtual bool isActive() const;
    [[nodiscard]] virtual bool isDestroyed() const;
    virtual void activate();
    virtual void deactivate();
    virtual void destroy();

    void bindFunction(std::function<void(Functional*, const std::string&, Character*)> func, const std::string& text,
                      bool is_usable_by_npc);
    virtual void use(Character* user);

private:
    bool is_active_;
    bool is_destroyed_;

    std::string activation_;
    std::vector<std::string> functions_, datas_;

    std::vector<std::function<void(Functional*, const std::string&, Character*)>> funcs_;

    const std::string* text_to_use_;

    bool is_usable_by_npc_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_FUNCTIONAL_H
