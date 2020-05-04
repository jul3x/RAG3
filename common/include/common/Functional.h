//
// Created by jul3x on 21.04.20.
//


#ifndef RAG3_COMMON_FUNCTIONAL_H
#define RAG3_COMMON_FUNCTIONAL_H

#include <functional>
#include <string>


using namespace r3e;

class Character;

class Functional : public Identifiable, public Unique {
public:
    Functional(const std::string& activation, const std::vector<std::string>& functions,
               const std::vector<std::string>& datas, const std::string& id, int u_id = -1);

    [[nodiscard]] const std::string& getActivation() const;

    [[nodiscard]] const std::string& getFunctionsStr() const;

    [[nodiscard]] const std::string& getDatasStr() const;

    [[nodiscard]] const std::vector<std::string>& getFunctions() const;

    [[nodiscard]] const std::vector<std::string>& getDatas() const;

    [[nodiscard]] const std::string& getTextToUse() const;

    void setActivation(const std::string& str);

    void setFunctionsStr(const std::string& str);

    void setDatasStr(const std::string& str);

    void bindFunction(std::function<void(Functional*, const std::string&, Character*)> func, const std::string& text,
                      bool is_usable_by_npc);

    [[nodiscard]] virtual bool isActive() const;

    virtual void deactivate();

    virtual void use(Character* user);

    [[nodiscard]] virtual bool isUsableByNPC() const;

private:
    bool is_active_;

    std::string activation_;
    std::vector<std::string> functions_, datas_;

    std::vector<std::function<void(Functional*, const std::string&, Character*)>> funcs_;

    const std::string* text_to_use_;

    bool is_usable_by_npc_;

};

#endif // RAG3_COMMON_FUNCTIONAL_H
