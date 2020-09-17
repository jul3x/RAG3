//
// Created by jul3x on 05.09.20.
//

#include <SFML/System/Vector2.hpp>
#include <R3E/j3x/J3XVisitor.h>

using namespace r3e::j3x;

void J3XVisitor::visitScript(Script* t) {} //abstract class
void J3XVisitor::visitDef(Def* t) {} //abstract class
void J3XVisitor::visitType(Type* t) {} //abstract class
void J3XVisitor::visitExpr(Expr* t) {} //abstract class

void J3XVisitor::visitProg(Prog *prog)
{
    prog->listdef_->accept(this);
}

void J3XVisitor::visitVarDef(VarDef *vardef)
{
    vardef->type_->accept(this);
    visitIdent(vardef->ident_);
    vardef->expr_->accept(this);

    bool int_to_float = vardef->type_->strtype_ == "float" and vardef->expr_->strtype_ == "int";

    if (vardef->type_->strtype_ == vardef->expr_->strtype_) {
        this->addNewVariable(vardef->ident_, vardef->expr_->value_, vardef->type_->strtype_, vardef->line_number_);
    }
    else if (int_to_float) {
        this->addNewVariable(vardef->ident_, static_cast<float>(std::any_cast<int>(vardef->expr_->value_)),
                vardef->type_->strtype_, vardef->line_number_);
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(vardef->line_number_) + " - Variable " + vardef->ident_ + " assign failed. Assigned type \"" +
            vardef->expr_->strtype_ + "\" not matching declared type \"" + vardef->type_->strtype_ + "\"!");
    }
}

void J3XVisitor::visitInt(Int *int_type)
{
    int_type->strtype_ = "int";
}

void J3XVisitor::visitStr(Str *str)
{
    str->strtype_ = "string";
}

void J3XVisitor::visitBool(Bool *bool_type)
{
    bool_type->strtype_ = "bool";
}

void J3XVisitor::visitFloat(Float *float_type)
{
    float_type->strtype_ = "float";
}

void J3XVisitor::visitVector(Vector *vector)
{
    vector->strtype_ = "vector";
}

void J3XVisitor::visitArrType(ArrType *arrtype)
{
    arrtype->strtype_ = "list";
}

void J3XVisitor::visitEAdd(EAdd *eadd)
{
    eadd->expr_1->accept(this);
    eadd->expr_2->accept(this);

    if (eadd->expr_1->strtype_ == "int" and eadd->expr_2->strtype_ == "int") {
        eadd->value_ = std::any_cast<int>(eadd->expr_1->value_) + std::any_cast<int>(eadd->expr_2->value_);
        eadd->strtype_ = "int";
    }
    else if (eadd->expr_1->strtype_ == "float" and eadd->expr_2->strtype_ == "int") {
        eadd->value_ = std::any_cast<float>(eadd->expr_1->value_) + static_cast<float>(std::any_cast<int>(eadd->expr_2->value_));
        eadd->strtype_ = "float";
    }
    else if (eadd->expr_1->strtype_ == "int" and eadd->expr_2->strtype_ == "float") {
        eadd->value_ = static_cast<float>(std::any_cast<int>(eadd->expr_1->value_)) + std::any_cast<float>(eadd->expr_2->value_);
        eadd->strtype_ = "float";
    }
    else if (eadd->expr_1->strtype_ == "float" and eadd->expr_2->strtype_ == "float") {
        eadd->value_ = std::any_cast<float>(eadd->expr_1->value_) + std::any_cast<float>(eadd->expr_2->value_);
        eadd->strtype_ = "float";
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(eadd->line_number_) + " - Addition can be performed only on numbers!");
    }
}

void J3XVisitor::visitESub(ESub *esub)
{
    esub->expr_1->accept(this);
    esub->expr_2->accept(this);

    if (esub->expr_1->strtype_ == "int" and esub->expr_2->strtype_ == "int") {
        esub->value_ = std::any_cast<int>(esub->expr_1->value_) - std::any_cast<int>(esub->expr_2->value_);
        esub->strtype_ = "int";
    }
    else if (esub->expr_1->strtype_ == "float" and esub->expr_2->strtype_ == "int") {
        esub->value_ = std::any_cast<float>(esub->expr_1->value_) - static_cast<float>(std::any_cast<int>(esub->expr_2->value_));
        esub->strtype_ = "float";
    }
    else if (esub->expr_1->strtype_ == "int" and esub->expr_2->strtype_ == "float") {
        esub->value_ = static_cast<float>(std::any_cast<int>(esub->expr_1->value_)) - std::any_cast<float>(esub->expr_2->value_);
        esub->strtype_ = "float";
    }
    else if (esub->expr_1->strtype_ == "float" and esub->expr_2->strtype_ == "float") {
        esub->value_ = std::any_cast<float>(esub->expr_1->value_) - std::any_cast<float>(esub->expr_2->value_);
        esub->strtype_ = "float";
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(esub->line_number_) + " - Subtraction can be performed only on numbers!");
    }
}

void J3XVisitor::visitEMul(EMul *emul)
{
    emul->expr_1->accept(this);
    emul->expr_2->accept(this);

    if (emul->expr_1->strtype_ == "int" and emul->expr_2->strtype_ == "int") {
        emul->value_ = std::any_cast<int>(emul->expr_1->value_) * std::any_cast<int>(emul->expr_2->value_);
        emul->strtype_ = "int";
    }
    else if (emul->expr_1->strtype_ == "float" and emul->expr_2->strtype_ == "int") {
        emul->value_ = std::any_cast<float>(emul->expr_1->value_) * static_cast<float>(std::any_cast<int>(emul->expr_2->value_));
        emul->strtype_ = "float";
    }
    else if (emul->expr_1->strtype_ == "int" and emul->expr_2->strtype_ == "float") {
        emul->value_ = static_cast<float>(std::any_cast<int>(emul->expr_1->value_)) * std::any_cast<float>(emul->expr_2->value_);
        emul->strtype_ = "float";
    }
    else if (emul->expr_1->strtype_ == "float" and emul->expr_2->strtype_ == "float") {
        emul->value_ = std::any_cast<float>(emul->expr_1->value_) * std::any_cast<float>(emul->expr_2->value_);
        emul->strtype_ = "float";
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(emul->line_number_) + " - Multiplication can be performed only on numbers!");
    }
}

void J3XVisitor::visitEDiv(EDiv *ediv)
{
    ediv->expr_1->accept(this);
    ediv->expr_2->accept(this);

    if (ediv->expr_1->strtype_ == "int" and ediv->expr_2->strtype_ == "int") {
        ediv->value_ = std::any_cast<int>(ediv->expr_1->value_) / std::any_cast<int>(ediv->expr_2->value_);
        ediv->strtype_ = "int";
    }
    else if (ediv->expr_1->strtype_ == "float" and ediv->expr_2->strtype_ == "int") {
        ediv->value_ = std::any_cast<float>(ediv->expr_1->value_) / static_cast<float>(std::any_cast<int>(ediv->expr_2->value_));
        ediv->strtype_ = "float";
    }
    else if (ediv->expr_1->strtype_ == "int" and ediv->expr_2->strtype_ == "float") {
        ediv->value_ = static_cast<float>(std::any_cast<int>(ediv->expr_1->value_)) / std::any_cast<float>(ediv->expr_2->value_);
        ediv->strtype_ = "float";
    }
    else if (ediv->expr_1->strtype_ == "float" and ediv->expr_2->strtype_ == "float") {
        ediv->value_ = std::any_cast<float>(ediv->expr_1->value_) / std::any_cast<float>(ediv->expr_2->value_);
        ediv->strtype_ = "float";
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(ediv->line_number_) + " - Division can be performed only on numbers!");
    }
}

void J3XVisitor::visitENeg(ENeg *eneg)
{
    eneg->expr_->accept(this);

    if (eneg->expr_->strtype_ == "int") {
        eneg->value_ = -std::any_cast<int>(eneg->expr_->value_);
        eneg->strtype_ = "int";
    }
    else if (eneg->expr_->strtype_ == "float") {
        eneg->value_ = -std::any_cast<float>(eneg->expr_->value_);
        eneg->strtype_ = "float";
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(eneg->line_number_) + " - Negation can be performed only on numbers!");
    }
}

void J3XVisitor::visitEList(EList *elist)
{
    elist->listexpr_->accept(this);

    elist->strtype_ = "list";

    std::vector<std::any> value;

    for (auto& expr : *elist->listexpr_) {
        value.emplace_back(expr->value_);
    }

    elist->value_ = value;
}

void J3XVisitor::visitELitInt(ELitInt *elitint)
{
    visitInteger(elitint->integer_);
    elitint->strtype_ = "int";
    elitint->value_ = elitint->integer_;
}

void J3XVisitor::visitEString(EString *estring)
{
    visitString(estring->string_);
    estring->strtype_ = "string";
    estring->value_ = estring->string_;
}

void J3XVisitor::visitEDouble(EDouble *edouble)
{
    visitDouble(edouble->double_);
    edouble->strtype_ = "float";
    edouble->value_ = static_cast<float>(edouble->double_);
}

void J3XVisitor::visitELitTrue(ELitTrue *elittrue)
{
    elittrue->strtype_ = "bool";
    elittrue->value_ = true;
}

void J3XVisitor::visitELitFalse(ELitFalse *elitfalse)
{
    elitfalse->strtype_ = "bool";
    elitfalse->value_ = false;
}

void J3XVisitor::visitEVar(EVar *evar)
{
    visitIdent(evar->ident_);
    evar->strtype_ = this->getVariableType(evar->ident_, evar->line_number_);
    evar->value_ = this->getVariableValue(evar->ident_, evar->line_number_);
}

void J3XVisitor::visitEVector(EVector *evector)
{
    evector->expr_1->accept(this);
    evector->expr_2->accept(this);
    evector->strtype_ = "vector";
    if (evector->expr_1->strtype_ == "float" and evector->expr_2->strtype_ == "float") {
        evector->value_ = sf::Vector2f(std::any_cast<float>(evector->expr_1->value_),
                                       std::any_cast<float>(evector->expr_2->value_));
    }
    else if (evector->expr_1->strtype_ == "int" and evector->expr_2->strtype_ == "float") {
        evector->value_ = sf::Vector2f(static_cast<float>(std::any_cast<int>(evector->expr_1->value_)),
                                       std::any_cast<float>(evector->expr_2->value_));
    }
    else if (evector->expr_1->strtype_ == "float" and evector->expr_2->strtype_ == "int") {
        evector->value_ = sf::Vector2f(std::any_cast<float>(evector->expr_1->value_),
                                       static_cast<float>(std::any_cast<int>(evector->expr_2->value_)));
    }
    else if (evector->expr_1->strtype_ == "int" and evector->expr_2->strtype_ == "int") {
        evector->value_ = sf::Vector2f(static_cast<float>(std::any_cast<int>(evector->expr_1->value_)),
                                       static_cast<float>(std::any_cast<int>(evector->expr_2->value_)));
    }
    else {
        throw std::invalid_argument("line number " + std::to_string(evector->line_number_) + " - Vector can be constructed only from numbers!");
    }
}


void J3XVisitor::visitListDef(ListDef* listdef)
{
  for (ListDef::iterator i = listdef->begin() ; i != listdef->end() ; ++i)
  {
    (*i)->accept(this);
  }
}

void J3XVisitor::visitListExpr(ListExpr* listexpr)
{
  for (ListExpr::iterator i = listexpr->begin() ; i != listexpr->end() ; ++i)
  {
    (*i)->accept(this);
  }
}


void J3XVisitor::visitInteger(Integer x)
{
  /* Code for Integer Goes Here */
}

void J3XVisitor::visitChar(Char x)
{
  /* Code for Char Goes Here */
}

void J3XVisitor::visitDouble(Double x)
{
  /* Code for Double Goes Here */
}

void J3XVisitor::visitString(String x)
{
  /* Code for String Goes Here */
}

void J3XVisitor::visitIdent(Ident x)
{
  /* Code for Ident Goes Here */
}



