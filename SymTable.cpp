//
// Created by Han Zhengyang on 2023/10/14.
//

#include "SymTable.h"
#include "ErrorHandler.h"
#include "Parser.h"
#include "SymTable.h"
#include "Lexer.h"
TableItem::TableItem(string ident, int type, int dim): ident_(ident), type_(type), dim_(dim){
}
TableItem::TableItem(string ident, int type, int dim, bool error): ident_(ident), type_(type), dim_(dim), error_(error){
}
Var::Var(string ident, int type, int isConst, int dim): TableItem(ident, type, dim), isConst_(isConst){

}
Var:: Var(Var* old):TableItem(old->ident_, old->type_, old->dim_, old->error_), isConst_(old->isConst_){
}
bool Var::getIsLoad() {
    return isLoad_;
}
void Var::setIsLoad(bool isLoad) {
    isLoad_ = isLoad;
}
Function::Function(string ident, int type, vector<int> & dims, int dim, int sum): TableItem(ident, type, dim), dims_(dims), sumOfParas_(sum){

}
Function::Function(Function *old):TableItem(old->ident_, old->type_, old->dim_, old->error_), dims_(old->dims_), sumOfParas_(old->sumOfParas_) {}
Exps::Exps(): TableItem("", VOID, UNDEFINEDIM){}
string TableItem::getIdent() {
    return ident_;
}
void TableItem::setVal(int value){
    val_ = value;
}
int TableItem::getVal(){
    return val_;
}
int TableItem:: getDim() {
    return dim_;
}
SymTable::SymTable(int id, int father): id_(id), father_(father) {}
bool SymTable:: isRedefine(string ident){
    int sum = items_.size();
    for(int i = 0; i < sum; i++) {
        if(items_[i]->ident_ == ident) {
            return true;
        }
    }
    return false;
}
bool SymTable::isContainIdent(string ident, string className) {
    for(int i = 0; i < items_.size(); i++) {
        if(items_[i]->ident_ == ident && typeid(items_[i]).name() == className) {
            return true;
        }
    }
    return false;
}
int SymTable::indexOf(string ident, int type) {
    for(int i = 0; i < items_.size(); i++) {
        if(items_[i]->type_ == INTNUM && type == INTNUM && ident == items_[i]->ident_){
            return i;
        }else if(type != INTNUM && items_[i]->type_ != INTNUM && items_[i]->ident_ == ident ) {
            return i;
        }
    }
    return -1;
}
void SymTable:: delItem(string ident) {
    for(int i = 0; i < items_.size(); i++) {
        if(items_[i]->ident_ == ident) {
            items_.erase(items_.begin() + i);
            return;
        }
    }
}